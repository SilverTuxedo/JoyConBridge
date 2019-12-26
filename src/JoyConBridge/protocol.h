#pragma once
#include <array>
#include "Buffer.h"


namespace joy_con_bridge::protocol
{
enum class LedState
{
	OFF,
	ON,
	FLASHING
};

enum class BatteryStatus : uint8_t
{
	EMPTY = 0,
	CRITICAL,
	LOW,
	MEDIUM,
	FULL
};

/*
 * Pointers are cast to the structs declared here, for easier access to parameters in buffers.
 * Therefore, these structs must match the buffer in memory, without any padding.
 */
#pragma pack(push, 1)

struct ButtonStatusLeft
{
	bool down : 1;
	bool up : 1;
	bool right : 1;
	bool left : 1;
	bool sr : 1;
	bool sl : 1;
	bool l : 1;
	bool zl : 1;
};

struct ButtonStatusRight
{
	bool y : 1;
	bool x : 1;
	bool b : 1;
	bool a : 1;
	bool sr : 1;
	bool sl : 1;
	bool r : 1;
	bool zr : 1;
};

struct ButtonStatusShared
{
	bool minus : 1;
	bool plus : 1;
	bool rightStick : 1;
	bool leftStick : 1;
	bool home : 1;
	bool capture : 1;
	bool unused : 1;
	bool chargingGrip : 1;
};

/*
 * Raw sensor data, without any calibration or scaling.
 */
struct SensorData
{
	int16_t accelerometer[3];
	int16_t gyroscope[3];
};

/*
 * Parameters passed when sending a command to read from SPI.
 */
struct SpiReadCommandParameters
{
	uint32_t readOffset;
	uint8_t readSize;
};

/*
 *All input reports start with this structure.
 */
struct InputReport
{
	uint8_t id;
	uint8_t timer;
	uint8_t connectionInfo : 4;
	bool batteryCharging : 1;
	BatteryStatus batteryStatus : 3;
	ButtonStatusRight buttonStatusRight;
	ButtonStatusShared buttonStatusShared;
	ButtonStatusLeft buttonStatusLeft;
	uint8_t leftAnalogStick[3];
	uint8_t rightAnalogStick[3];
	uint8_t rumble;
};

/*
 * Input reports of type 0x21.
 */
struct StandardInputReport : InputReport
{
	uint8_t dataType : 7;
	bool ack : 1;
	uint8_t replyToSubcommandId;
	uint8_t data[34];
};

/*
 * Input reports of type 0x30.
 */
struct StandardFullInputReport : InputReport
{
	SensorData sensorData[3];
};

#pragma pack(pop)


static_assert(0x31 == sizeof(StandardInputReport),
	"Struct is not in the correct size - Standard reports can't be parsed");
static_assert(0x31 == sizeof(StandardFullInputReport),
	"Struct is not in the correct size - Standard Full reports can't be parsed");
static_assert(5 == sizeof(SpiReadCommandParameters),
	"Struct is not in the correct size - SPI Read can't be called");

/**
	@brief Builds a buffer in the structure of a command for a JoyCon.

	@param[in] commandId The ID of the command.
	@param[in, optional] commandData Additional data of the command.
	@param[in, optional] isBluetooth true if the connection is Bluetooth based.

	@return A buffer that represents the command.
*/
Buffer getCommandBuffer(uint8_t commandId, const Buffer& commandData = {}, bool isBluetooth = true);


/**
	@brief Builds a buffer in the structure of a command + sub command for a JoyCon.

	@param[in] commandId The ID of the command.
	@param[in] subCommandId The ID of the sub command.
	@param[in, optional] commandData Additional data of the command.
	@param[in, optional] isBluetooth true if the connection is Bluetooth based.

	@return A buffer that represents the command.
*/
Buffer getSubCommandBuffer(uint8_t commandId, uint8_t subCommandId, const Buffer& commandData = {},
                           bool isBluetooth = true);

/**
	@brief Builds the LED sequence that corresponds to the given input.

	@param states The states of each of the individual LEDs.

	@return The corresponding sequence.
*/
uint8_t getLedSequence(const std::array<LedState, 4>& states);

/**
	@brief Decodes the analog stick data.

	@param[in] stickDataArray A 3 byte long array with the raw stick values.

	@return Decoded values in the horizontal and vertical axis.
*/
std::array<uint16_t, 2> decodeAnalogStick(const uint8_t* stickDataArray);

/**
	@brief Decodes calibration data for analog sticks.
	The meaning of each index in the returned array depends on the analog stick (left/right).

	@param[in] calibrationDataArray A 9 byte log array with the raw calibration data.

	@return Decoded calibration data.
*/
std::array<uint16_t, 6> decodeStickCalibrationData(const uint8_t* calibrationDataArray);
}
