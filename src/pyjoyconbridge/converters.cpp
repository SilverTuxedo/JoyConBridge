#include "converters.h"

namespace joy_con_bridge::python
{
boost::python::dict buttonsStateToDict(const ButtonsState& state)
{
	boost::python::dict result;

	result["a"]          = state.a;
	result["b"]          = state.b;
	result["x"]          = state.x;
	result["y"]          = state.y;
	result["r"]          = state.r;
	result["zr"]         = state.zr;
	result["rightStick"] = state.rightStick;
	result["plus"]       = state.plus;
	result["home"]       = state.home;
	result["srRight"]    = state.srRight;
	result["slRight"]    = state.slRight;
	result["up"]         = state.up;
	result["down"]       = state.down;
	result["left"]       = state.left;
	result["right"]      = state.right;
	result["l"]          = state.l;
	result["zl"]         = state.zl;
	result["leftStick"]  = state.leftStick;
	result["minus"]      = state.minus;
	result["capture"]    = state.capture;
	result["srLeft"]     = state.srLeft;
	result["slLeft"]     = state.slLeft;

	return result;
}

boost::python::dict analogStickToDict(const AnalogStick& stick)
{
	boost::python::dict result;

	result["x"] = stick.x;
	result["y"] = stick.y;

	return result;
}

boost::python::dict threeAxesSensorToDict(const ThreeAxesSensor& sensor)
{
	boost::python::dict result;

	result["x"] = sensor.x;
	result["y"] = sensor.y;
	result["z"] = sensor.z;

	return result;
}
}
