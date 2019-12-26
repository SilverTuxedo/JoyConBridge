#include <stdexcept>
#include "strings.h"


namespace joy_con_bridge::strings
{
std::string wideToChar(const std::wstring& target)
{
	std::string result(target.size() + 1, '\0');
	if (0 > snprintf(result.data(), result.size(), "%ls", target.data())) {
		throw std::runtime_error("Unable to convert string");
	}
	result.resize(result.size() - 1); // removes NUL added by snprintf
	return result;
}
}
