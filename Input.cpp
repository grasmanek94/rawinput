#include "Input.h"

#include <iostream>
//#include <memory>
#include <sstream>

//using std::auto_ptr;
using std::wstring;
using std::wstringstream;
using std::vector;

using std::wcout;

RawInput::Input::Input(const HWND hwnd)
	: ri_registered_devices_(0),
	ri_mice_(),
	ri_mouse_(),
	ri_keyboard_(),
	ri_hid_()
{
	try {
		vector<RAWINPUTDEVICELIST> & ri_sys_dev(this->GetSystemDevices());

		const USHORT usage_array[] = {
			HID_DEVICE_SYSTEM_MOUSE, //RIM_TYPEMOUSE
			HID_DEVICE_SYSTEM_KEYBOARD, //RIM_TYPEKEYBOARD
			HID_DEVICE_SYSTEM_GAME, //RIM_TYPEHID
		};

//		unsigned int pcbSize;

//		auto_ptr<RID_DEVICE_INFO> pData(new RID_DEVICE_INFO);

		for (unsigned int i = 0; i < ri_sys_dev.size(); ++i) {
// 			::GetRawInputDeviceInfo(
// 				ri_sys_dev[i].hDevice,
// 				RIDI_DEVICENAME,
// 				0,
// 				&pcbSize);
// 
// 			vector<wchar_t> dev_data(pcbSize);
// 
// 			::GetRawInputDeviceInfo(
// 				ri_sys_dev[i].hDevice,
// 				RIDI_DEVICENAME,
// 				&dev_data[0],
// 				&pcbSize);

			wstring & device_name = this->GetDeviceName(ri_sys_dev[i].hDevice);

			if (device_name.find(TEXT("RDP_")) == -1) { //Look for non-RDP devices
#ifdef _DEBUG
				wcout << TEXT("Device[") << i << TEXT("]: ") << device_name << TEXT("\n\n");
#endif
// 				if (::GetRawInputDeviceInfo(
// 					ri_sys_dev[i].hDevice,
// 					RIDI_DEVICEINFO,
// 					0,
// 					&pcbSize)) throw Input_Exception(TEXT("GetRawInputDeviceInfo()"), GetLastError());

// 				pcbSize = sizeof(RID_DEVICE_INFO);
// 				RID_DEVICE_INFO pData;
// 
// 				//If uiCommand is RIDI_DEVICEINFO, set RID_DEVICE_INFO.cbSize to
// 				//sizeof(RID_DEVICE_INFO) before calling GetRawInputDeviceInfo.
// 				pData.cbSize = sizeof(RID_DEVICE_INFO);
// 
// 				//Returns bytes copied to pData, gets input data in pData.
// 				if (::GetRawInputDeviceInfo(
// 					ri_sys_dev[i].hDevice,
// 					RIDI_DEVICEINFO,
// 					&pData,
// 					&pcbSize) < 0) throw Input_Exception(TEXT("GetRawInputDeviceInfo()"), GetLastError());

				RID_DEVICE_INFO pData(this->GetDeviceInfo(ri_sys_dev[i].hDevice));

				//if (pData->dwType == RIM_TYPEHID) continue;

				RAWINPUTDEVICE ri_dev = {
					HID_USAGE_PAGE,
					usage_array[pData.dwType],
					0L, //RIDEV_INPUTSINK,
					hwnd
				};

				ri_registered_devices_.push_back(ri_dev);
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
	for (unsigned int i = 0; i < ri_registered_devices_.size(); ++i) {
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

RID_DEVICE_INFO RawInput::Input::GetDeviceInfo(const HANDLE & hDevice)
{
	UINT pcbSize = sizeof(RID_DEVICE_INFO);
	RID_DEVICE_INFO pData;

	pData.cbSize = sizeof(RID_DEVICE_INFO);

	if (::GetRawInputDeviceInfo(
		hDevice,
		RIDI_DEVICEINFO,
		&pData,
		&pcbSize) < 0) throw Input_Exception(TEXT("GetRawInputDeviceInfo()"), GetLastError());

	return pData;
}

wstring RawInput::Input::GetDeviceName(const HANDLE & hDevice)
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

	return wstring(&dev_data[0]);
}

LRESULT  RawInput::Input::Update(HWND & hWnd, UINT & message, WPARAM & wParam, LPARAM & lParam)
{
	//HRAWINPUT hRawInput = reinterpret_cast<HRAWINPUT>(lParam);

	unsigned int pcbSize(sizeof(RAWINPUT)); // no magic numbers
	//unsigned int result;

	// Returns 0 if correct, in pcbSize return buffer size
	//result = ::GetRawInputData(hRawInput, RID_INPUT, 0, &pcbSize, sizeof(RAWINPUTHEADER));

	//if (result != 0) throw Input_Exception(TEXT("GetRawInputData()"), GetLastError());

	//std::auto_ptr<RAWINPUT> pData(new RAWINPUT);
	RAWINPUT pData;

	// Returns -1 on fail. Returns input data in pData.
	unsigned int r = ::GetRawInputData(
		reinterpret_cast<HRAWINPUT>(lParam),
		RID_INPUT,
		&pData,
		&pcbSize,
		sizeof(RAWINPUTHEADER));

	if (r == -1) throw Input_Exception(TEXT("GetRawInputData()"), GetLastError());

	switch (pData.header.dwType) {
		case RIM_TYPEMOUSE:
			ri_mouse_ = ri_mice_[pData.header.hDevice] << pData;
#ifdef _DEBUG
			wcout << pData.header.hDevice << L' ' << ri_mouse_.data_str();
#endif
			break;
		case RIM_TYPEKEYBOARD:
			ri_keyboard_ = ri_keybs_[pData.header.hDevice] << pData;
#ifdef _DEBUG
			wcout << pData.header.hDevice << L' ' << ri_keyboard_.data_str();
#endif
			break;
		case RIM_TYPEHID:
			ri_hids_[pData.header.hDevice] = ri_hid_ << pData;
#ifdef _DEBUG
			wcout << pData.header.hDevice << L' ' << ri_hids_[pData.header.hDevice].data_str();
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

/*LRESULT RawInput::Input::UpdateBuff(const LPARAM & lParam) //does not work, old
{
	HRAWINPUT	hRawInput	 = (HRAWINPUT)lParam;
	RAWINPUT	* pData		 = 0;
	UINT		pcbSize		 = 0U;
	static UINT	cbSizeHeader = sizeof(RAWINPUTHEADER);

	int raw_data;

	if (raw_data = ::GetRawInputBuffer(0, &pcbSize, cbSizeHeader)) return;
	
	pData = (RAWINPUT *)::malloc(pcbSize);

	if ((raw_data = ::GetRawInputBuffer(pData, &pcbSize, cbSizeHeader)) == -1) {
		::free(pData);
		throw Input_Exception(TEXT("GetRawInputBuffer()"), GetLastError());
	}

	RAWINPUT * j = pData;

	for (int i = 0; i < raw_data; ++i) {
		switch (pData->header.dwType) {
			case RIM_TYPEMOUSE:
				ri_mouse = j->data.mouse;
				break;
			case RIM_TYPEKEYBOARD:
				ri_keyboard = j->data.keyboard;
				break;
			case RIM_TYPEHID:
				ri_hid = j->data.hid;
				break;
		}
		NEXTRAWINPUTBLOCK(j); //Broken on x64?
	}

	return ::DefRawInputProc(&pData, raw_data, cbSizeHeader);
}*/

LRESULT RawInput::Input::Change(WPARAM & wParam, LPARAM & lParam)
{
	switch (wParam) {
		case GIDC_ARRIVAL: return 0;
		case GIDC_REMOVAL:
			RID_DEVICE_INFO pData(this->GetDeviceInfo(reinterpret_cast<HANDLE>(lParam)));

			switch (pData.dwType) {
				case RIM_TYPEMOUSE:
					ri_mice_.erase(ri_mice_.find(reinterpret_cast<HANDLE>(lParam)));
					break;
				case RIM_TYPEKEYBOARD:
					ri_keybs_.erase(ri_keybs_.find(reinterpret_cast<HANDLE>(lParam)));
					break;
				case RIM_TYPEHID:
					ri_hids_.erase(ri_hids_.find(reinterpret_cast<HANDLE>(lParam)));
					break;
			}
	}
	return 0;
}

void RawInput::Input::Clean(void)
{
// 	ri_mouse_		= RAWMOUSE();
// 	ri_keyboard_	= RAWKEYBOARD();
// 	ri_hid_			= RAWHID();
}

bool RawInput::Input::KeyUp(const unsigned short & button)
{
	return ri_keyboard_.ri_data_.VKey == button
		&& (ri_keyboard_.ri_data_.Flags == RI_KEY_BREAK || ri_keyboard_.ri_data_.Flags == RI_KEY_BREAK + RI_KEY_E0);
}

bool RawInput::Input::KeyDown(const unsigned short & button)
{
	return ri_keyboard_.ri_data_.VKey == button
		&& (ri_keyboard_.ri_data_.Flags == RI_KEY_MAKE || ri_keyboard_.ri_data_.Flags == RI_KEY_MAKE + RI_KEY_E0);
}

bool RawInput::Input::KeyHeld(const unsigned short & button)
{
	return ri_keyboard_.ri_data_.VKey == button;
}

bool RawInput::Input::MouseButtonHeld(const unsigned long & button)
{
	return ri_mouse_.ri_data_.ulRawButtons == button;
}

bool RawInput::Input::MouseEvent(const unsigned short & f)
{
	return ri_mouse_.ri_data_.usButtonFlags == f;
}

void RawInput::Input::MousePos(long & x, long & y)
{
	x = ri_mouse_.ri_data_.lLastX;
	y = ri_mouse_.ri_data_.lLastY;

	ri_mouse_.Clean();
}