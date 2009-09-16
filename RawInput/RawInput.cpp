#include "RawInput.h"

#include <iostream>
#include <sstream>

using std::map;
using std::wstring;
using std::wstringstream;
using std::vector;

using std::wcout;

RawInput::Input::Input(const HWND hwnd)
	: ri_registered_devices_(),
	ri_mouse_(),
	ri_keyboard_(),
	ri_hid_(),
	ri_devs_()
{
	try {
		vector<RAWINPUTDEVICELIST> & ri_sys_dev = this->GetSystemDevices();

		const USHORT usage_array[] = {
			HID_DEVICE_SYSTEM_MOUSE,	//RIM_TYPEMOUSE == 0
			HID_DEVICE_SYSTEM_KEYBOARD,	//RIM_TYPEKEYBOARD == 1
			HID_DEVICE_SYSTEM_GAME,		//RIM_TYPEHID == 2
		};

		const DWORD flags_array[] = {
			0,					// Flags for mouse
			RIDEV_NOHOTKEYS,	// Flags for keyboard
			0,					// Flags for HID
		};

		for (unsigned int i = 0; i < ri_sys_dev.size(); ++i) {
			wstring & device_name = this->GetDeviceName(ri_sys_dev[i].hDevice);

			if (device_name.find(TEXT("RDP_")) == -1) { //Look for non-RDP devices
#ifdef _DEBUG
				wcout << TEXT("Device[") << i << TEXT("]: ") << device_name << TEXT("\n\n");
#endif
				RID_DEVICE_INFO pData(this->GetDeviceInfo(ri_sys_dev[i].hDevice));

				RAWINPUTDEVICE ri_dev = {
					HID_USAGE_PAGE,
					usage_array[pData.dwType],
					flags_array[pData.dwType],
					hwnd
				};

				ri_registered_devices_.push_back(ri_dev);

				ri_devs_[ri_sys_dev[i].hDevice] = this->MakeDevice(pData.dwType);
			}
		}

		if (!::RegisterRawInputDevices(
			&ri_registered_devices_[0],
			ri_registered_devices_.size(),
			sizeof(RAWINPUTDEVICE))) throw Input_Exception(TEXT("RegisterRawInputDevices()"), GetLastError());
	}
	catch (Input_Exception & e) {
		wstringstream ss;
		
		ss << e.error_msg << TEXT(" has thrown an exception. Code: ") << e.hresult;

		::MessageBox(
			hwnd,
			ss.str().c_str(),
			TEXT("RawInput"),
			MB_ICONERROR);
	}
}

RawInput::Input::~Input(void)
{
	for (map<HANDLE, Device_base *>::iterator i = ri_devs_.begin(), end = ri_devs_.end()
		; i != end
		; ++i)
		delete i->second;

	for (unsigned int i = 0
		; i < ri_registered_devices_.size()
		; ++i) {
		ri_registered_devices_[i].dwFlags = RIDEV_REMOVE;
		ri_registered_devices_[i].hwndTarget = 0;
	}

	::RegisterRawInputDevices(
		&ri_registered_devices_[0],
		ri_registered_devices_.size(),
		sizeof(RAWINPUTDEVICE));
}

vector<RAWINPUTDEVICELIST> RawInput::Input::GetSystemDevices(void) const
{
	unsigned int ri_system_count;

	//Returns the number of RAWINPUTDEVICELIST structures that can be
	//contained in the buffer to which pRawInputDeviceList points
	//If this value is less than the number of devices attached to the
	//system, the function returns the actual number of devices in this
	//variable and fails with ERROR_INSUFFICIENT_BUFFER.
	if (::GetRawInputDeviceList(
		0,
		&ri_system_count,
		sizeof(RAWINPUTDEVICELIST)) < 0) throw Input_Exception(TEXT("GetRawInputDeviceList()"), ::GetLastError());

	vector<RAWINPUTDEVICELIST> ri_sys_dev(ri_system_count);

	//Populates ri_sys_dev with the number of devices attached to the system.
	if (::GetRawInputDeviceList(
		&ri_sys_dev[0],
		&ri_system_count,
		sizeof(RAWINPUTDEVICELIST)) < 0) throw Input_Exception(TEXT("GetRawInputDeviceList()"), ::GetLastError());

	return ri_sys_dev;
}

RID_DEVICE_INFO RawInput::Input::GetDeviceInfo(const HANDLE & hDevice) const
{
	unsigned int pcbSize = sizeof(RID_DEVICE_INFO);
	RID_DEVICE_INFO pData;

	// If uiCommand is RIDI_DEVICEINFO, set RID_DEVICE_INFO.cbSize to
	// sizeof(RID_DEVICE_INFO) before calling GetRawInputDeviceInfo.
	pData.cbSize = sizeof(RID_DEVICE_INFO);

	// Returns bytes copied to pData, gets input data in pData.
	if (::GetRawInputDeviceInfo(
		hDevice,
		RIDI_DEVICEINFO,
		&pData,
		&pcbSize) < 0) throw Input_Exception(TEXT("GetRawInputDeviceInfo()"), GetLastError());

	return pData;
}

wstring RawInput::Input::GetDeviceName(const HANDLE & hDevice) const
{
	unsigned int pcbSize;

	::GetRawInputDeviceInfo(
		hDevice,
		RIDI_DEVICENAME,
		0,
		&pcbSize);

	vector<wchar_t> dev_data(pcbSize);

	::GetRawInputDeviceInfo(
		hDevice,
		RIDI_DEVICENAME,
		&dev_data[0],
		&pcbSize);

	return &dev_data[0];
}

RawInput::Device_base * RawInput::Input::MakeDevice(const DWORD & t)
{
	switch (t) {
		case RIM_TYPEMOUSE: return new Device<RAWMOUSE>;
		case RIM_TYPEKEYBOARD: return new Device<RAWKEYBOARD>;
		case RIM_TYPEHID: return new Device<RAWHID>;
	}

	throw Input_Exception(TEXT("Bad device."));
}

LRESULT  RawInput::Input::Update(HWND & hWnd, UINT & message, WPARAM & wParam, LPARAM & lParam)
{
	unsigned int pcbSize(sizeof(RAWINPUT));
	RAWINPUT rData;

	try {
		// Returns -1 on fail. Returns input data in pData.
		if (::GetRawInputData(
			reinterpret_cast<HRAWINPUT>(lParam),
			RID_INPUT,
			&rData,
			&pcbSize,
			sizeof(RAWINPUTHEADER)) == -1) throw Input_Exception(TEXT("GetRawInputData()"), GetLastError());
	}
	catch (Input_Exception & e) {
#ifdef _DEBUG
		wstringstream ss;

		ss << e.error_msg << TEXT(" has thrown an exception. Code: ") << e.hresult;

		::MessageBox(
			hWnd,
			ss.str().c_str(),
			TEXT("RawInput"),
			MB_ICONERROR);
#endif
		this->Clean();
	}

	//*ri_devs_[rData.header.hDevice] << rData;

	switch (rData.header.dwType) {
		case RIM_TYPEMOUSE: ri_mouse_ << rData;
#ifdef _DEBUG
			wcout << rData.header.hDevice << ' ' << ri_mouse_;
#endif
			break;
		case RIM_TYPEKEYBOARD: ri_keyboard_ << rData;
#ifdef _DEBUG
			wcout << rData.header.hDevice << ' ' << ri_keyboard_;
#endif
			break;
		case RIM_TYPEHID: ri_hid_ << rData;
#ifdef _DEBUG
			wcout << rData.header.hDevice << ' ' << ri_hid_;
#endif
			break;
	}

	switch (GET_RAWINPUT_CODE_WPARAM(wParam)) {
		case RIM_INPUT:
		case RIM_INPUTSINK:
			return ::DefWindowProc(hWnd, message, wParam, lParam);
		default:
			return 0;
	}
}

LRESULT RawInput::Input::UpdateBuff(HWND & hWnd, UINT & message, WPARAM & wParam, LPARAM & lParam)
{
	unsigned int pcbSize;

	// If pData is NULL, the minimum required buffer, in bytes, is returned in *pcbSize.
	unsigned int rBuff = ::GetRawInputBuffer(0, &pcbSize, sizeof(RAWINPUTHEADER));

	if (rBuff == -1) throw Input_Exception(TEXT("GetRawInputBuffer()"), GetLastError());

	RAWINPUT * rData = (RAWINPUT *)::malloc(pcbSize);

	// If rData is not NULL and the function is successful, the return
	// value is the number of RAWINPUT structures written to pData.
	rBuff = ::GetRawInputBuffer(rData, &pcbSize, sizeof(RAWINPUTHEADER));

	if (rBuff < 0) {
		::free(rData);
		throw Input_Exception(TEXT("GetRawInputBuffer()"), GetLastError());
	}

	if (rBuff == 0) {
		::free(rData);
		return 0;
	}

	RAWINPUT * j = rData;

	for (UINT i = 0; i < rBuff; ++i) {
		switch (j->header.dwType) {
			case RIM_TYPEMOUSE:
				ri_mouse_ << *j;
				break;
			case RIM_TYPEKEYBOARD:
				ri_keyboard_ << *j;
				break;
			case RIM_TYPEHID:
				ri_hid_ << *j;
				break;
		}
		NEXTRAWINPUTBLOCK(j); // Broken on x64?
	}

	::free(rData);

	switch (GET_RAWINPUT_CODE_WPARAM(wParam)) {
		case RIM_INPUT:
		case RIM_INPUTSINK:
			return ::DefWindowProc(hWnd, message, wParam, lParam);
		default:
			return 0;
	}
}

LRESULT RawInput::Input::Change(WPARAM & wParam, LPARAM & lParam)
{
	switch (wParam) {
		case GIDC_ARRIVAL:
			// Handle arrival
			 return 0;
		case GIDC_REMOVAL:
			// Handle removal
			 return 0;
	}
	return 0;
}

void RawInput::Input::Clean(void)
{
	ri_mouse_.Clean();
	ri_keyboard_.Clean();
	ri_hid_.Clean();
}

bool RawInput::Input::KeyUp(const unsigned short & key)
{
	return ri_keyboard_.GetData().VKey == key
		&& (ri_keyboard_.GetData().Flags == RI_KEY_BREAK || ri_keyboard_.GetData().Flags == RI_KEY_BREAK + RI_KEY_E0);
}

bool RawInput::Input::KeyDown(const unsigned short & key)
{
	return ri_keyboard_.GetData().VKey == key
		&& (ri_keyboard_.GetData().Flags == RI_KEY_MAKE || ri_keyboard_.GetData().Flags == RI_KEY_MAKE + RI_KEY_E0);
}

bool RawInput::Input::KeyHeld(const unsigned short & key)
{
	return ri_keyboard_.GetData().VKey == key
		&& (ri_keyboard_.GetData().Flags != RI_KEY_BREAK || ri_keyboard_.GetData().Flags != RI_KEY_BREAK + RI_KEY_E0);
}

bool RawInput::Input::MouseButtonHeld(const unsigned long & button)
{
	return ri_mouse_.GetData().ulRawButtons == button;
}

bool RawInput::Input::MouseButton(const unsigned short & flag)
{
	return ri_mouse_.GetData().usButtonFlags == flag;
}

void RawInput::Input::MousePos(long & x, long & y)
{
	x = ri_mouse_.GetData().lLastX;
	y = ri_mouse_.GetData().lLastY;
}