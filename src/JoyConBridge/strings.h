#pragma once
#include <string>


namespace joy_con_bridge::strings
{
/**
	@brief Converts a wide character string (wchar_t) to a character string (char).

	@param[in] target The string to convert.

	@return The converted string.

	@throw runtime_error If the string can't be converted.
*/
std::string wideToChar(const std::wstring& target);
}
