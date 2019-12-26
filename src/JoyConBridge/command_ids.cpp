#include "command_ids.h"


namespace joy_con_bridge::command_ids
{
const uint8_t COMMAND_START_SUBCOMMAND = 0x1;

const uint8_t SUBCOMMAND_RUMBLE_CONTROL       = 0x48;
const uint8_t SUBCOMMAND_OPTION_RUMBLE_ENABLE = 0x1;

const uint8_t SUBCOMMAND_IMU_CONTROL       = 0x40;
const uint8_t SUBCOMMAND_OPTION_IMU_ENABLE = 0x1;

const uint8_t SUBCOMMAND_REPORT_MODE                   = 0x3;
const uint8_t SUBCOMMAND_OPTION_REPORT_MODE_FULL       = 0x30;
const uint8_t SUBCOMMAND_OPTION_REPORT_MODE_NFC        = 0x31;
const uint8_t SUBCOMMAND_OPTION_REPORT_MODE_SIMPLE_HID = 0x3F;

const uint8_t SUBCOMMAND_SET_PLAYER_LED = 0x30;

const uint8_t SUBCOMMAND_SPI_READ = 0x10;

const uint8_t PACKET_TYPE_STANDARD        = 0x21;
const uint8_t PACKET_TYPE_BUTTONS_AND_IMU = 0x30;
const uint8_t PACKET_TYPE_NFC             = 0x31;
}
