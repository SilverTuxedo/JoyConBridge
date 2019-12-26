#pragma once
#include <vector>


namespace joy_con_bridge
{
using Buffer = std::vector<uint8_t>;

/**
	@brief Dumps a type into the end of a buffer.
	For example, dumping (uint32)0x12345678 would result in the buffer being appended with {0x78, 0x56, 0x34, 0x12}.

	@param[in] buffer The buffer to dump the value into.
	@param[in] value The value to dump
*/
template <typename T>
void dumpToBuffer(Buffer& buffer, const T& value)
{
	const auto startAddress = reinterpret_cast<const uint8_t*>(&value);
	buffer.insert(buffer.end(), startAddress, startAddress + sizeof(T));
}
}
