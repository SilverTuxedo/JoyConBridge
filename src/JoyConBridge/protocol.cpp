#include "protocol.h"


namespace joy_con_bridge::protocol
{
Buffer getCommandBuffer(uint8_t commandId, const Buffer& commandData, bool isBluetooth)
{
	Buffer commandBuffer;

	if (!isBluetooth) {
		commandBuffer = {0x80, 0x92, 0, 0x31, 0, 0, 0, 0};
	}

	commandBuffer.push_back(commandId);
	commandBuffer.insert(commandBuffer.end(), commandData.begin(), commandData.end());

	return commandBuffer;
}

Buffer getSubCommandBuffer(uint8_t commandId, uint8_t subCommandId, const Buffer& commandData, bool isBluetooth)
{
	static int globalTimer = 0;

	Buffer subCommandBuffer = {
		static_cast<uint8_t>((++globalTimer) % 0x10),
		0,
		1,
		0x40,
		0x40,
		0,
		1,
		0x40,
		0x40
	}; // Rumble data is required for each subcommand.

	subCommandBuffer.push_back(subCommandId);
	subCommandBuffer.insert(subCommandBuffer.end(), commandData.begin(), commandData.end());

	return getCommandBuffer(commandId, subCommandBuffer, isBluetooth);
}

uint8_t getLedSequence(const std::array<LedState, 4>& states)
{
	uint8_t ledSequence = 0;
	for (size_t i = 0; i < states.size(); ++i) {
		uint8_t currentBitmask = 0;
		if (LedState::ON == states[i]) {
			currentBitmask = 1 << i;
		} else if (LedState::FLASHING == states[i]) {
			currentBitmask = 0x10 << i;
		}
		ledSequence |= currentBitmask;
	}

	return ledSequence;
}

std::array<uint16_t, 2> decodeAnalogStick(const uint8_t* stickDataArray)
{
	const uint16_t horizontal = stickDataArray[0] | ((stickDataArray[1] & 0xF) << 8);
	const uint16_t vertical = (stickDataArray[1] >> 4) | (stickDataArray[2] << 4);
	return {horizontal, vertical};
}

std::array<uint16_t, 6> decodeStickCalibrationData(const uint8_t* calibrationDataArray)
{
	std::array<uint16_t, 6> result{};

	result[0] = (calibrationDataArray[1] << 8) & (0xF00 | calibrationDataArray[0]);
	result[1] = (calibrationDataArray[2] << 4) | (calibrationDataArray[1] >> 4);
	result[2] = (calibrationDataArray[4] << 8) & (0xF00 | calibrationDataArray[3]);
	result[3] = (calibrationDataArray[5] << 4) | (calibrationDataArray[4] >> 4);
	result[4] = (calibrationDataArray[7] << 8) & (0xF00 | calibrationDataArray[6]);
	result[5] = (calibrationDataArray[8] << 4) | (calibrationDataArray[7] >> 4);

	return result;
}
}
