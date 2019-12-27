#include "connect.h"
#include "exceptions.h"


namespace joy_con_bridge::connect
{
const auto JOYCON_VENDOR_ID = 0x57e;
const auto JOYCON_L_PRODUCT_ID = 0x2006;
const auto JOYCON_R_PRODUCT_ID = 0x2007;

JoyCon getLeftJoyCon()
{
	return JoyCon(HidDevice(JOYCON_VENDOR_ID, JOYCON_L_PRODUCT_ID), Hand::LEFT);
}

JoyCon getRightJoyCon()
{
	return JoyCon(HidDevice(JOYCON_VENDOR_ID, JOYCON_R_PRODUCT_ID), Hand::RIGHT);
}
}
