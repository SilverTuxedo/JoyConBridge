#pragma once
#include <boost/python.hpp>
#include "JoyCon.h"

/*
 * Converter hook macros generate replacement functions for a class function.
 * The hook allows seamless conversion of the returned type from a C++ native type to something Python can understand more easily.
 * The generated functions are meant to be used where the original function would be when defining the Python bindings.
 */

#define CONVERTER_HOOK_NAME(functionName) converterProxy_##functionName
/**
	Defines a hook function.

	@param Type The relevant class.
	@param functionName The name of the function that the hook should replace/call. The function must be a member of the class.
	@param converter The converter function to use.
*/
#define CONVERTER_HOOK_DEFINITION(Type, functionName, converter) auto CONVERTER_HOOK_NAME(functionName)(Type* instance) { return (converter)(instance->functionName()); }

namespace joy_con_bridge::python
{
boost::python::dict buttonsStateToDict(const ButtonsState& state);

boost::python::dict analogStickToDict(const AnalogStick& stick);

boost::python::dict threeAxesSensorToDict(const ThreeAxesSensor& sensor);
}
