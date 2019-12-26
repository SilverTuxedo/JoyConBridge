#include <boost/python.hpp>
#include "connect.h"
#include "converters.h"
#include "JoyCon.h"

using namespace joy_con_bridge;

CONVERTER_HOOK_DEFINITION(JoyCon, getButtonsState, python::buttonsStateToDict)
CONVERTER_HOOK_DEFINITION(JoyCon, getLeftStick, python::analogStickToDict)
CONVERTER_HOOK_DEFINITION(JoyCon, getRightStick, python::analogStickToDict)
CONVERTER_HOOK_DEFINITION(JoyCon, getAccelerometer, python::threeAxesSensorToDict)
CONVERTER_HOOK_DEFINITION(JoyCon, getGyroscope, python::threeAxesSensorToDict)

BOOST_PYTHON_MODULE(pyjoyconbridge)
{
	using namespace boost::python;

	class_<HidDevice>("HidDevice", init<std::string>("Opens the HID device in the given path."))
		.def(init<unsigned short, unsigned short>("Opens the HID device with the specified Vendor ID and Product ID."))
		.def(init<unsigned short, unsigned short, std::wstring>(
			"Opens the HID device with the specified Vendor ID, Product ID and serial number."));

	enum_<protocol::LedState>("LedState", "Possible state of a single JoyCon player LED")
		.value("OFF", protocol::LedState::OFF)
		.value("ON", protocol::LedState::ON)
		.value("FLASHING", protocol::LedState::FLASHING);

	enum_<Hand>("Hand")
		.value("NONE", Hand::NONE)
		.value("LEFT", Hand::LEFT)
		.value("RIGHT", Hand::RIGHT);

	def("get_left_joy_con", &connect::getLeftJoyCon);
	def("get_right_joy_con", &connect::getRightJoyCon);

	class_<JoyCon>("JoyCon", init<HidDevice>("Creates a new JoyCon from the given HID device."))
		.def(init<HidDevice, Hand>("Creates a new JoyCon from the given HID device."))
		.def("poll", &JoyCon::poll)
		.add_property("buttons_state", &CONVERTER_HOOK_NAME(getButtonsState))
		.add_property("left_stick", &CONVERTER_HOOK_NAME(getLeftStick))
		.add_property("right_stick", &CONVERTER_HOOK_NAME(getRightStick))
		.add_property("gyroscope", &CONVERTER_HOOK_NAME(getGyroscope))
		.add_property("accelerometer", &CONVERTER_HOOK_NAME(getAccelerometer))
		.add_property("likely_hand", &JoyCon::getLikelyHand)
		.def("set_player_leds_by_number", &JoyCon::setPlayerLedsByNumber)
		.def("set_player_leds", &JoyCon::setPlayerLeds);
}
