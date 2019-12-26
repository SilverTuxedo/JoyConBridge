#pragma once
#include <memory>
#include <string>
#include "Buffer.h"
#include "hidapi/hidapi.h"


namespace joy_con_bridge
{
class HidDevice
{
	// This class suffers from a severe case of RAS syndrome.
public:
	/**
		@brief Takes ownership of an existing, unwrapped, HID device pointer.

		@param[in] device The device to take ownership on.
	 */
	explicit HidDevice(hid_device* device);

	/**
		@brief Opens the HID device in the given path.

		@param[in] path The path to the HID device to open.

		@throws HidOpenError
	 */
	explicit HidDevice(const std::string& path);

	/**
		@brief Opens the HID device with the given Vendor ID (VID) and Product ID (PID).
		If there are multiple, only one of them is opened.

		@param[in] vendorId The Vendor ID (VID) of the device to open.
		@param[in] productId The Product ID (PID) of the device to open.
	 */
	HidDevice(unsigned short vendorId, unsigned short productId);

	/**
		@brief Opens the HID device with the given Vendor ID (VID), Product ID (PID) and serial number.

		@param[in] vendorId The Vendor ID (VID) of the device to open.
		@param[in] productId The Product ID (PID) of the device to open.
		@param[in] serialNumber The Serial Number of the device to open.
	 */
	HidDevice(unsigned short vendorId, unsigned short productId, const std::wstring& serialNumber);

	/**
		Writes data to the device.

		@param[in] buffer The data to write.

		@return The number of bytes written.

		@throw HidError if writing fails.
	*/
	size_t write(const Buffer& buffer);

	/**
		Reads data from the device.

		@param[in] maxReadSize The read length limit.

		@return The data read from the device.

		@throw HidError if reading fails.
	*/
	Buffer read(size_t maxReadSize);

	/**
		Reads data from the device, but waits for data no longer than a specific amount of time.

		@param[in] maxReadSize The read length limit.
		@param[in] milliseconds The maximum amount of time, in milliseconds, to wait for data.

		@return The data read from the device.

		@throw HidError if reading fails.
		@throw HidTimeoutError if the timeout is reached.
	*/
	Buffer readTimeout(size_t maxReadSize, int milliseconds);

protected:
	/// RAII wrapper for HID (device) pointers from hidapi.
	using HidDevicePointer = std::shared_ptr<hid_device>;
private:
	/**
		@brief Open a HID device using a Vendor ID (VID), Product ID (PID) and a serial number.

		The first device with the specified VID and PID is opened.

		@param[in] vendorId The Vendor ID (VID) of the device to open.
		@param[in] productId The Product ID (PID) of the device to open.

		@return A pointer to an HID device.

		@throw HidOpenError if the device can't be opened.
	*/
	static HidDevicePointer open(unsigned short vendorId, unsigned short productId);

	/**
		@brief Open a HID device using a Vendor ID (VID), Product ID (PID) and a serial number.

		@param[in] vendorId The Vendor ID (VID) of the device to open.
		@param[in] productId The Product ID (PID) of the device to open.
		@param[in] serialNumber The Serial Number of the device to open.

		@return A pointer to an HID device.

		@throw HidOpenError if the device can't be opened.
	*/
	static HidDevicePointer open(unsigned short vendorId, unsigned short productId, const std::wstring& serialNumber);

	/**
		@brief Open a HID device using a path.

		@param[in] path The path of the device to open.

		@return A pointer to an HID device.

		@throw HidOpenError if the device can't be opened.
	*/
	static HidDevicePointer openPath(const std::string& path);

	HidDevicePointer m_device;
};
}
