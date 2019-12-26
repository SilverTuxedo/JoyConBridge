#pragma once
#include <stdexcept>
#include <string>
#include "HidDevice.h"


namespace joy_con_bridge
{
class JoyConError : public std::exception
{};

class JoyConNotResponding : public JoyConError
{
	char const* what() const override;
};

class HidError : public std::exception
{
public:
	explicit HidError(hid_device* device);

	char const* what() const override;

protected:
	std::string m_error;
};

class HidTimeoutError : public HidError
{
public:
	explicit HidTimeoutError(hid_device* device);
};

class HidOpenError : public HidError
{
public:
	HidOpenError();

	char const* what() const override;
};
}
