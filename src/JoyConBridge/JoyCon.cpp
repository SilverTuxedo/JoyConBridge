#include <algorithm>
#include "JoyCon.h"
#include "command_ids.h"
#include "exceptions.h"
#include "hidapi.h"
#include "HidDevice.h"
#include "protocol.h"


using namespace joy_con_bridge::command_ids;

namespace joy_con_bridge
{
const float PI = 3.141592654f;

JoyCon::JoyCon(HidDevice device, Hand hand)
	: m_device(std::move(device))
	, m_buttons{}
	, m_leftStick{}
	, m_rightStick{}
	, m_gyroscope{}
	, m_accelerometer{}
	, m_calibrationData{}
	, m_likelyHand(hand)
{
	sendSubcommand(SUBCOMMAND_REPORT_MODE, {SUBCOMMAND_OPTION_REPORT_MODE_SIMPLE_HID});
	updateCalibrationData();
	sendSubcommand(SUBCOMMAND_RUMBLE_CONTROL, {SUBCOMMAND_OPTION_RUMBLE_ENABLE});
	sendSubcommand(SUBCOMMAND_IMU_CONTROL, {SUBCOMMAND_OPTION_IMU_ENABLE});
	sendSubcommand(SUBCOMMAND_REPORT_MODE, {SUBCOMMAND_OPTION_REPORT_MODE_FULL});
}

void JoyCon::poll()
{
	static const auto READ_TIMEOUT = 5000;

	Buffer reportBuffer;
	do {
		try {
			reportBuffer = m_device.readTimeout(sizeof(protocol::StandardFullInputReport), READ_TIMEOUT);
		} catch (HidTimeoutError&) {
			throw JoyConNotResponding();
		}
	} while (
		reportBuffer[0] != PACKET_TYPE_STANDARD &&
		reportBuffer[0] != PACKET_TYPE_BUTTONS_AND_IMU &&
		reportBuffer[0] != PACKET_TYPE_NFC
	);

	auto report = reinterpret_cast<protocol::StandardFullInputReport*>(reportBuffer.data());
	updateButtons(report);
	updateAnalogSticks(report);
	updateSensors(report);
}

ButtonsState JoyCon::getButtonsState() const
{
	return m_buttons;
}

AnalogStick JoyCon::getLeftStick() const
{
	return m_leftStick;
}

AnalogStick JoyCon::getRightStick() const
{
	return m_rightStick;
}

ThreeAxesSensor JoyCon::getGyroscope() const
{
	return m_gyroscope;
}

ThreeAxesSensor JoyCon::getAccelerometer() const
{
	return m_accelerometer;
}

Hand JoyCon::getLikelyHand() const
{
	return m_likelyHand;
}

void JoyCon::setPlayerLedsByNumber(unsigned int playerNumber)
{
	using protocol::LedState;

	switch (playerNumber) {
	case 1: // *---
		setPlayerLeds(LedState::ON, LedState::OFF, LedState::OFF, LedState::OFF);
		break;
	case 2: // **--
		setPlayerLeds(LedState::ON, LedState::ON, LedState::OFF, LedState::OFF);
		break;
	case 3: // ***-
		setPlayerLeds(LedState::ON, LedState::ON, LedState::ON, LedState::OFF);
		break;
	case 4: // ****
		setPlayerLeds(LedState::ON, LedState::ON, LedState::ON, LedState::ON);
		break;
	case 5: // *--*
		setPlayerLeds(LedState::ON, LedState::OFF, LedState::OFF, LedState::ON);
		break;
	case 6: // *-*-
		setPlayerLeds(LedState::ON, LedState::OFF, LedState::ON, LedState::OFF);
		break;
	case 7: // *-**
		setPlayerLeds(LedState::ON, LedState::OFF, LedState::ON, LedState::ON);
		break;
	case 8: // -**-
		setPlayerLeds(LedState::OFF, LedState::ON, LedState::ON, LedState::OFF);
		break;
	default:
		setPlayerLeds(LedState::FLASHING, LedState::FLASHING, LedState::FLASHING, LedState::FLASHING);
	}
}

void JoyCon::setPlayerLeds(protocol::LedState led1, protocol::LedState led2, protocol::LedState led3,
                           protocol::LedState led4)
{
	const uint8_t ledSequence = protocol::getLedSequence({led1, led2, led3, led4});
	sendSubcommand(SUBCOMMAND_SET_PLAYER_LED, {ledSequence});
}

Buffer JoyCon::sendSubcommand(uint8_t subcommandId, const Buffer& commandData)
{
	static const auto PACKET_SKIP_LIMIT = 100; // The limit to the amount of garbage packets that is acceptable.
	const auto command                  = protocol::getSubCommandBuffer(COMMAND_START_SUBCOMMAND, subcommandId,
	                                                                    commandData);
	m_device.write(command);

	size_t packetsRead = 0;
	// Wait until receiving an ACK.
	try {
		while (PACKET_SKIP_LIMIT > packetsRead) {
			auto response = m_device.readTimeout(sizeof(protocol::StandardFullInputReport), 500);
			++packetsRead;
			auto report = reinterpret_cast<protocol::StandardInputReport*>(response.data());
			if (PACKET_TYPE_STANDARD == report->id && report->ack && subcommandId == report->replyToSubcommandId) {
				if (0 == report->dataType) {
					// simple ACK, no readResult.
					return Buffer();
				}
				return Buffer(report->data, report->data + sizeof(report->data));
			}
		}
	} catch (const HidTimeoutError&) {
		// intentionally empty
	}

	throw JoyConNotResponding();
}

Buffer JoyCon::readSpi(uint32_t offset, uint8_t size)
{
	const protocol::SpiReadCommandParameters parameters = {offset, size};
	Buffer parametersBuffer;
	dumpToBuffer(parametersBuffer, parameters);

	const Buffer response = sendSubcommand(SUBCOMMAND_SPI_READ, parametersBuffer);
	// the response echos the parameter buffer, we're skipping that readResult.
	const auto actualDataStart = response.cbegin() + parametersBuffer.size();
	return Buffer(actualDataStart, actualDataStart + size);
}

void JoyCon::updateButtons(const protocol::StandardFullInputReport* report)
{
	m_buttons.a          = report->buttonStatusRight.a;
	m_buttons.b          = report->buttonStatusRight.b;
	m_buttons.x          = report->buttonStatusRight.x;
	m_buttons.y          = report->buttonStatusRight.y;
	m_buttons.r          = report->buttonStatusRight.r;
	m_buttons.zr         = report->buttonStatusRight.zr;
	m_buttons.rightStick = report->buttonStatusShared.rightStick;
	m_buttons.plus       = report->buttonStatusShared.plus;
	m_buttons.home       = report->buttonStatusShared.home;
	m_buttons.srRight    = report->buttonStatusRight.sr;
	m_buttons.slRight    = report->buttonStatusRight.sl;
	m_buttons.up         = report->buttonStatusLeft.up;
	m_buttons.down       = report->buttonStatusLeft.down;
	m_buttons.left       = report->buttonStatusLeft.left;
	m_buttons.right      = report->buttonStatusLeft.right;
	m_buttons.l          = report->buttonStatusLeft.l;
	m_buttons.zl         = report->buttonStatusLeft.zl;
	m_buttons.leftStick  = report->buttonStatusShared.leftStick;
	m_buttons.minus      = report->buttonStatusShared.minus;
	m_buttons.capture    = report->buttonStatusShared.capture;
	m_buttons.srLeft     = report->buttonStatusLeft.sr;
	m_buttons.slLeft     = report->buttonStatusLeft.sl;
}

void JoyCon::updateCalibrationData()
{
	static const auto FACTORY_CALIBRATION_LEFT_STICK_OFFSET  = 0x603D;
	static const auto FACTORY_CALIBRATION_RIGHT_STICK_OFFSET = 0x6046;
	static const auto FACTORY_CALIBRATION_SENSORS_OFFSET     = 0x6020;
	static const auto USER_CALIBRATION_LEFT_STICK_OFFSET     = 0x8012;
	static const auto USER_CALIBRATION_RIGHT_STICK_OFFSET    = 0x801D;
	static const auto USER_CALIBRATION_SENSORS_OFFSET        = 0x8028;
	static const auto STICK_CALIBRATION_DATA_SIZE            = 9;
	static const auto SENSOR_CALIBRATION_DATA_SIZE           = 2 * sizeof(ThreeAxesCalibrationData);

	std::optional<Buffer> optionalUserData = readUserCalibrationData(USER_CALIBRATION_LEFT_STICK_OFFSET,
	                                                                 STICK_CALIBRATION_DATA_SIZE);
	Buffer leftStickCalibrationData;
	if (optionalUserData) {
		leftStickCalibrationData = std::move(*optionalUserData);
	} else {
		leftStickCalibrationData = readSpi(FACTORY_CALIBRATION_LEFT_STICK_OFFSET, STICK_CALIBRATION_DATA_SIZE);
	}

	optionalUserData = readUserCalibrationData(USER_CALIBRATION_RIGHT_STICK_OFFSET, STICK_CALIBRATION_DATA_SIZE);
	Buffer rightStickCalibrationData;
	if (optionalUserData) {
		rightStickCalibrationData = std::move(*optionalUserData);
	} else {
		rightStickCalibrationData = readSpi(FACTORY_CALIBRATION_RIGHT_STICK_OFFSET, STICK_CALIBRATION_DATA_SIZE);
	}

	optionalUserData = readUserCalibrationData(USER_CALIBRATION_SENSORS_OFFSET, SENSOR_CALIBRATION_DATA_SIZE);
	Buffer sensorsCalibrationData;
	if (optionalUserData) {
		sensorsCalibrationData = std::move(*optionalUserData);
	} else {
		sensorsCalibrationData = readSpi(FACTORY_CALIBRATION_SENSORS_OFFSET, SENSOR_CALIBRATION_DATA_SIZE);
	}

	const auto factoryCalibrationValuesLeft = protocol::decodeStickCalibrationData(leftStickCalibrationData.data());
	updateLeftStickCalibrationData(factoryCalibrationValuesLeft);

	const auto factoryCalibrationValuesRight = protocol::decodeStickCalibrationData(rightStickCalibrationData.data());
	updateRightStickCalibrationData(factoryCalibrationValuesRight);

	const auto rawAccelerometerDataStart = sensorsCalibrationData.data();
	const auto rawGyroscopeDataStart     = rawAccelerometerDataStart + sizeof(ThreeAxesCalibrationData);
	ThreeAxesCalibrationData data{};
	std::memcpy(&data, rawAccelerometerDataStart, sizeof(data));
	updateAccelerometerCalibrationData(data);
	std::memcpy(&data, rawGyroscopeDataStart, sizeof(data));
	updateGyroscopeCalibrationData(data);
}

void JoyCon::updateLeftStickCalibrationData(const std::array<uint16_t, 6>& rawCalibrationValues)
{
	m_calibrationData.leftStick.x.center         = rawCalibrationValues[2];
	m_calibrationData.leftStick.x.maxAboveCenter = m_calibrationData.leftStick.x.center + rawCalibrationValues[0];
	m_calibrationData.leftStick.x.minBelowCenter = m_calibrationData.leftStick.x.center - rawCalibrationValues[4];

	m_calibrationData.leftStick.y.center         = rawCalibrationValues[3];
	m_calibrationData.leftStick.y.maxAboveCenter = m_calibrationData.leftStick.y.center + rawCalibrationValues[1];
	m_calibrationData.leftStick.y.minBelowCenter = m_calibrationData.leftStick.y.center - rawCalibrationValues[5];
}

void JoyCon::updateRightStickCalibrationData(const std::array<uint16_t, 6>& rawCalibrationValues)
{
	m_calibrationData.rightStick.x.center         = rawCalibrationValues[0];
	m_calibrationData.rightStick.x.maxAboveCenter = m_calibrationData.rightStick.x.center + rawCalibrationValues[4];
	m_calibrationData.rightStick.x.minBelowCenter = m_calibrationData.rightStick.x.center - rawCalibrationValues[2];

	m_calibrationData.rightStick.y.center         = rawCalibrationValues[1];
	m_calibrationData.rightStick.y.maxAboveCenter = m_calibrationData.rightStick.y.center + rawCalibrationValues[5];
	m_calibrationData.rightStick.y.minBelowCenter = m_calibrationData.rightStick.y.center - rawCalibrationValues[3];
}

void JoyCon::updateAccelerometerCalibrationData(const ThreeAxesCalibrationData& data)
{
	static const float G = 9.8f;

	m_calibrationData.accelerometerCoeff = {
		1.0f / static_cast<float>(data.sensitivityOffset.x - data.neutral.x) * 4 * G,
		1.0f / static_cast<float>(data.sensitivityOffset.y - data.neutral.y) * 4 * G,
		1.0f / static_cast<float>(data.sensitivityOffset.z - data.neutral.z) * 4 * G
	};
}

void JoyCon::updateGyroscopeCalibrationData(const ThreeAxesCalibrationData& data)
{
	static const float ONE_DEGREE_IN_RADIANS = PI / 180;

	m_calibrationData.gyroscopeCoeff = {
		1.0f / static_cast<float>(data.sensitivityOffset.x - data.neutral.x) * ONE_DEGREE_IN_RADIANS,
		1.0f / static_cast<float>(data.sensitivityOffset.y - data.neutral.y) * ONE_DEGREE_IN_RADIANS,
		1.0f / static_cast<float>(data.sensitivityOffset.z - data.neutral.z) * ONE_DEGREE_IN_RADIANS
	};
}

std::optional<Buffer> JoyCon::readUserCalibrationData(uint32_t offset, uint8_t size)
{
	static const uint16_t DATA_EXISTS_MAGIC = 0xA1B2;

	auto readResult            = readSpi(offset - sizeof(DATA_EXISTS_MAGIC), size + sizeof(DATA_EXISTS_MAGIC));
	const auto startMagicValue = *(reinterpret_cast<uint16_t*>(readResult.data()));
	if (DATA_EXISTS_MAGIC != startMagicValue) {
		return std::nullopt;
	}

	const auto calibrationDataStart = readResult.begin() + sizeof(DATA_EXISTS_MAGIC);
	const auto calibrationDataEnd   = calibrationDataStart + size;
	return Buffer(calibrationDataStart, calibrationDataEnd);
}

void JoyCon::updateAnalogSticks(const protocol::StandardFullInputReport* report)
{
	const auto leftStickValues = protocol::decodeAnalogStick(report->leftAnalogStick);
	if (isRawAnalogStickDataValid(leftStickValues)) {
		m_leftStick = getCalibratedStickValues(leftStickValues, m_calibrationData.leftStick);
	}

	const auto rightStickValues = protocol::decodeAnalogStick(report->rightAnalogStick);
	if (isRawAnalogStickDataValid(rightStickValues)) {
		m_rightStick = getCalibratedStickValues(rightStickValues, m_calibrationData.rightStick);
	}
}

void JoyCon::updateSensors(const protocol::StandardFullInputReport* report)
{
	m_accelerometer.x = static_cast<float>(report->sensorData[0].accelerometer[0]) * m_calibrationData
	                                                                                 .accelerometerCoeff.x;
	m_accelerometer.y = static_cast<float>(report->sensorData[0].accelerometer[1]) * m_calibrationData
	                                                                                 .accelerometerCoeff.y;
	m_accelerometer.z = static_cast<float>(report->sensorData[0].accelerometer[2]) * m_calibrationData
	                                                                                 .accelerometerCoeff.z;

	m_gyroscope.x = static_cast<float>(report->sensorData[0].gyroscope[0]) * m_calibrationData.gyroscopeCoeff.x;
	m_gyroscope.y = static_cast<float>(report->sensorData[0].gyroscope[1]) * m_calibrationData.gyroscopeCoeff.y;
	m_gyroscope.z = static_cast<float>(report->sensorData[0].gyroscope[2]) * m_calibrationData.gyroscopeCoeff.z;
}

AnalogStick JoyCon::getCalibratedStickValues(const std::array<uint16_t, 2>& stickValues,
                                             const AnalogStickCalibrationData& calibrationData)
{
	static const float DEAD_ZONE_CENTER = 0.15f;
	static const float DEAD_ZONE_OUTER = 0.10f;
	static const float LEGAL_RANGE = 1.0f - DEAD_ZONE_OUTER - DEAD_ZONE_CENTER;

	float x, y;

	const uint16_t clampedX = std::clamp(stickValues[0], calibrationData.x.minBelowCenter,
	                                     calibrationData.x.maxAboveCenter);
	const uint16_t clampedY = std::clamp(stickValues[1], calibrationData.y.minBelowCenter,
	                                     calibrationData.y.maxAboveCenter);

	if (clampedX >= calibrationData.x.center) {
		x = static_cast<float>(clampedX - calibrationData.x.center) / 
			static_cast<float>(calibrationData.x.maxAboveCenter - calibrationData.x.center);
	} else {
		x = -(static_cast<float>(clampedX - calibrationData.x.center) /
			  static_cast<float>(calibrationData.x.minBelowCenter - calibrationData.x.center));
	}
	if (clampedY >= calibrationData.y.center) {
		y = static_cast<float>(clampedY - calibrationData.y.center) / 
			static_cast<float>(calibrationData.y.maxAboveCenter - calibrationData.y.center);
	} else {
		y = -(static_cast<float>(clampedY - calibrationData.y.center) /
			  static_cast<float>(calibrationData.y.minBelowCenter - calibrationData.y.center));
	}

	const float magnitude = std::sqrtf(x * x + y * y);
	if (magnitude > DEAD_ZONE_CENTER) {
		const float normalizedMagnitude = std::min(1.0f, (magnitude - DEAD_ZONE_CENTER) / LEGAL_RANGE);
		const float scale = normalizedMagnitude / magnitude;

		return {x * scale, y * scale};
	} else {
		return {0, 0};
	}
}

bool JoyCon::isRawAnalogStickDataValid(const std::array<uint16_t, 2>& stickValues)
{
	return !(0 == stickValues[0] && 0 == stickValues[1]);
}
}
