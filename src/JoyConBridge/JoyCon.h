#pragma once
#include <array>
#include <optional>
#include "Buffer.h"
#include "HidDevice.h"
#include "protocol.h"


namespace joy_con_bridge
{
struct AnalogStickCalibrationData
{
	struct
	{
		uint16_t maxAboveCenter;
		uint16_t center;
		uint16_t minBelowCenter;
	} x;

	struct
	{
		uint16_t maxAboveCenter;
		uint16_t center;
		uint16_t minBelowCenter;
	} y;
};

struct Position3D
{
	int16_t x;
	int16_t y;
	int16_t z;
};

struct Coefficient
{
	float x;
	float y;
	float z;
};

struct ThreeAxesCalibrationData
{
	Position3D neutral;
	Position3D sensitivityOffset;
};

struct CalibrationData
{
	AnalogStickCalibrationData leftStick;
	AnalogStickCalibrationData rightStick;
	Coefficient accelerometerCoeff;
	Coefficient gyroscopeCoeff;
};

struct AnalogStick
{
	float x;
	float y;
};

struct ThreeAxesSensor
{
	float x;
	float y;
	float z;
};

struct ButtonsState
{
	bool a;
	bool b;
	bool x;
	bool y;
	bool r;
	bool zr;
	bool rightStick;
	bool plus;
	bool home;
	bool srRight; // == SR on right joycon.
	bool slRight;

	bool up;
	bool down;
	bool left;
	bool right;
	bool l;
	bool zl;
	bool leftStick;
	bool minus;
	bool capture;
	bool srLeft;
	bool slLeft;
};


enum class ConnectionType
{
	BLUETOOTH,
	USB
};

enum class Hand
{
	NONE,
	LEFT,
	RIGHT
};

class JoyCon
{
public:
	/**
		@brief Constructs a JoyCon based on data from the given HID device.
	*/
	explicit JoyCon(HidDevice device, Hand hand = Hand::NONE);

	/**
		@brief Reads data from the joy con and updates buttons and sensors.

		@throws JoyConNotResponding If the JoyCon is not responding.
		@throws HidError If an internal HID error occurs.
	*/
	void poll();

	ButtonsState getButtonsState() const;

	AnalogStick getLeftStick() const;

	AnalogStick getRightStick() const;

	ThreeAxesSensor getGyroscope() const;

	ThreeAxesSensor getAccelerometer() const;

	Hand getLikelyHand() const;

	void setPlayerLeds(protocol::LedState led1, protocol::LedState led2, protocol::LedState led3,
	                   protocol::LedState led4);

	/**
		@brief Sets the player LED state according to the player number, like an actual Switch does. This supports players 1-8.

		@param[in] playerNumber the player number to indicate. If not in range 1-8, all LEDs are set to flash.
	*/
	void setPlayerLedsByNumber(unsigned int playerNumber);

private:
	/**
		@brief Sends a subcommand to the JoyCon.

		@param[in] subcommandId The ID of the subcommand.
		@param[in] commandData A buffer containing the command data/parameters.

		@return The data the subcommand returned. In case of a simple ACK, an empty buffer is returned.

		@throws JoyConNotResponding If an ACK is not received.
	*/
	Buffer sendSubcommand(uint8_t subcommandId, const Buffer& commandData);

	/**
		@brief Reads SPI data.

		@param[in] offset The offset to read from.
		@param[in] size The number of bytes to read. In practice, this is limited to 0x1D bytes.

		@return The data that was read.

		@throws See `JoyCon::sendSubcommand`.
	*/
	Buffer readSpi(uint32_t offset, uint8_t size);

	/**
		@brief Updates the state of all buttons based on the report.

		@param[in] report The report used to update the buttons.
	*/
	void updateButtons(const protocol::StandardFullInputReport* report);

	/**
		@brief Updates the calibration data of the analog sticks and buttons by reading from the JoyCon's SPI.
	*/
	void updateCalibrationData();

	void updateLeftStickCalibrationData(const std::array<uint16_t, 6>& rawCalibrationValues);

	void updateRightStickCalibrationData(const std::array<uint16_t, 6>& rawCalibrationValues);

	void updateAccelerometerCalibrationData(const ThreeAxesCalibrationData& data);

	void updateGyroscopeCalibrationData(const ThreeAxesCalibrationData& data);

	/**
		@brief Attempts to read user calibration data from the given offset. If the user data start magic value is missing, no data is returned.

		@param offset The offset in SPI that the user calibration data starts in.
		@param size The number of bytes to read.

		@return The user calibration data as it appears in SPI, if it exists.
	*/
	std::optional<Buffer> readUserCalibrationData(uint32_t offset, uint8_t size);

	/**
		@brief: Updates analog stick values based on an input report and existing calibration data.

		@param[in] report The latest input report.
	*/
	void updateAnalogSticks(const protocol::StandardFullInputReport* report);

	/**
		@brief: Updates sensor data based on an input report and existing calibration data.

		@param[in] report The latest input report.
	*/
	void updateSensors(const protocol::StandardFullInputReport* report);

	/**
		@brief Calculates the calibrated values of an analog stick.

		Credit to Hypersect (Ryan Juckett)
		http://blog.hypersect.com/interpreting-analog-sticks/
		Special thanks to CTCaer/jc_toolkit

		@param[in] stickValues The uncalibrated values of the analog stick.
		@param[in] calibrationData The calibration data to use.

		@return Calibrated values.
	*/
	static AnalogStick getCalibratedStickValues(const std::array<uint16_t, 2>& stickValues,
	                                            const AnalogStickCalibrationData& calibrationData);

	/**
		@brief Checks if analog stick values, before calibration, are valid.

		@param[in] stickValues The uncalibrated values.

		@return True if the values are valid
	*/
	static bool isRawAnalogStickDataValid(const std::array<uint16_t, 2>& stickValues);

	HidDevice m_device;
	ConnectionType m_connectionType = ConnectionType::BLUETOOTH; // Only bluetooth communication is supported.

	ButtonsState m_buttons;
	AnalogStick m_leftStick;
	AnalogStick m_rightStick;
	ThreeAxesSensor m_gyroscope;
	ThreeAxesSensor m_accelerometer;
	CalibrationData m_calibrationData;
	Hand m_likelyHand;
};
}
