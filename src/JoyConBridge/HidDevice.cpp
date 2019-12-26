#include "HidDevice.h"
#include "exceptions.h"


namespace joy_con_bridge
{
HidDevice::HidDevice(hid_device* device)
	: m_device{device, &hid_close}
{}

HidDevice::HidDevice(const std::string& path)
	: m_device{openPath(path)}
{}

HidDevice::HidDevice(unsigned short vendorId, unsigned short productId)
	: m_device{open(vendorId, productId)}
{}

HidDevice::HidDevice(unsigned short vendorId, unsigned short productId, const std::wstring& serialNumber)
	: m_device{open(vendorId, productId, serialNumber)}
{}

HidDevice::HidDevicePointer HidDevice::open(unsigned short vendorId, unsigned short productId)
{
	hid_device* device = hid_open(vendorId, productId, nullptr);
	if (nullptr == device) {
		throw HidOpenError();
	}

	return HidDevicePointer{device, &hid_close};
}

HidDevice::HidDevicePointer HidDevice::open(unsigned short vendorId, unsigned short productId,
                                            const std::wstring& serialNumber)
{
	hid_device* device = hid_open(vendorId, productId, serialNumber.data());
	if (nullptr == device) {
		throw HidOpenError();
	}

	return HidDevicePointer{device, &hid_close};
}

HidDevice::HidDevicePointer HidDevice::openPath(const std::string& path)
{
	hid_device* device = hid_open_path(path.data());
	if (nullptr == device) {
		throw HidOpenError();
	}

	return HidDevicePointer{device, &hid_close};
}

size_t HidDevice::write(const Buffer& buffer)
{
	const int writtenBytes = hid_write(m_device.get(), buffer.data(), buffer.size());
	if (0 > writtenBytes) {
		throw HidError(m_device.get());
	}

	return writtenBytes;
}

Buffer HidDevice::read(size_t maxReadSize)
{
	Buffer readData(maxReadSize);
	const int readDataLength = hid_read(m_device.get(), readData.data(), maxReadSize);
	if (0 > readDataLength) {
		throw HidError(m_device.get());
	}

	readData.resize(readDataLength);
	return readData;
}

Buffer HidDevice::readTimeout(size_t maxReadSize, int milliseconds)
{
	Buffer readData(maxReadSize);

	const int readDataLength = hid_read_timeout(m_device.get(), readData.data(), maxReadSize, milliseconds);
	if (0 == readDataLength) {
		throw HidTimeoutError(m_device.get());
	}
	if (-1 == readDataLength) {
		throw HidError(m_device.get());
	}
	readData.resize(readDataLength);
	return readData;
}
}
