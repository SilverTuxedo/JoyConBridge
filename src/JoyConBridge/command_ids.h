#pragma once
#include <cstdint>


namespace joy_con_bridge::command_ids
{
// Controls the JoyCon/alters its behavior.
extern const uint8_t COMMAND_START_SUBCOMMAND;

// Enables/disables rumble. Used with COMMAND_START_SUBCOMMAND.
extern const uint8_t SUBCOMMAND_RUMBLE_CONTROL;
// Enables rumble when used with SUBCOMMAND_RUMBLE_CONTROL.
extern const uint8_t SUBCOMMAND_OPTION_RUMBLE_ENABLE;

// Enables/disables gyroscope and accelerometer data. Used with COMMAND_START_SUBCOMMAND.
extern const uint8_t SUBCOMMAND_IMU_CONTROL;
// Enables gyroscope/accelerometer when used with SUBCOMMAND_IMU_CONTROL.
extern const uint8_t SUBCOMMAND_OPTION_IMU_ENABLE;

// Changes the report mode (which affects data rate). Used with COMMAND_START_SUBCOMMAND.
extern const uint8_t SUBCOMMAND_REPORT_MODE;
// Switches to the standard, full report mode (60Hz, button & stick & sensor data) when used with SUBCOMMAND_REPORT_MODE.
extern const uint8_t SUBCOMMAND_OPTION_REPORT_MODE_FULL;
// Switches to the NFC report mode (large packets at 60Hz) when used with SUBCOMMAND_REPORT_MODE.
extern const uint8_t SUBCOMMAND_OPTION_REPORT_MODE_NFC;
// Switches to the normal HID mode when used with SUBCOMMAND_REPORT_MODE.
extern const uint8_t SUBCOMMAND_OPTION_REPORT_MODE_SIMPLE_HID;

// Sets the JoyCon's player LED state. 
extern const uint8_t SUBCOMMAND_SET_PLAYER_LED;

// Reads SPI data. Parameters are size (uint32) and bytes (uint8)
extern const uint8_t SUBCOMMAND_SPI_READ;

// The magic number in the beginning of packets reporting button state and subcommand replies.
extern const uint8_t PACKET_TYPE_STANDARD;
// The magic number in the beginning of packets reporting button and IMU state.
extern const uint8_t PACKET_TYPE_BUTTONS_AND_IMU;
// The magic number in the beginning of packets reporting NFC data.
extern const uint8_t PACKET_TYPE_NFC;
}
