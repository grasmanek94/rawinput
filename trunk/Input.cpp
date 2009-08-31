#include "Input.h"

#include <iostream>
#include <memory>
#include <sstream>

using std::auto_ptr;
using std::wstring;
using std::wstringstream;
using std::vector;

using std::wcout;

RawInput::Input::Input(const HWND hwnd) try
	: ri_devices_(0),
	ri_mouse_(RAWMOUSE()),
	ri_keyboard_(RAWKEYBOARD()),
	ri_hid_(RAWHID())
{
	vector<RAWINPUTDEVICELIST> & ri_sys_dev(this->GetSystemDevices());

	unsigned int r;

	const unsigned short usage_array[] = {
		HID_DEVICE_SYSTEM_MOUSE, //RIM_TYPEMOUSE
		HID_DEVICE_SYSTEM_KEYBOARD, //RIM_TYPEKEYBOARD
		HID_DEVICE_SYSTEM_GAME, //RIM_TYPEHID
	};

	auto_ptr<RID_DEVICE_INFO> pData(new RID_DEVICE_INFO);

	unsigned int pcbSize;

	//Add asserts/exceptions!

	for (unsigned int i = 0; i < ri_sys_dev.size(); ++i) {
		::GetRawInputDeviceInfo(
			ri_sys_dev[i].hDevice,
			RIDI_DEVICENAME,
			0,
			&pcbSize);

		vector<wchar_t> rdp_dev_name(pcbSize);

		::GetRawInputDeviceInfo(
			ri_sys_dev[i].hDevice,
			RIDI_DEVICENAME,
			&rdp_dev_name[0],
			&pcbSize);

		wstring rdp_device_name(&rdp_dev_name[0]);

		if (rdp_device_name.find(TEXT("RDP_")) == -1) { //Look for non-RDP devices
#ifdef _DEBUG
			wcout << TEXT("Device[") << i << TEXT("]: ") << rdp_device_name << TEXT("\n\n"); //Debug only
#endif
			r = ::GetRawInputDeviceInfo(
				ri_sys_dev[i].hDevice,
				RIDI_DEVICEINFO,
				0,
				&pcbSize);

			if (r != 0) throw Input_Exception(TEXT("GetRawInputDeviceInfo()"), GetLastError());

			//If uiCommand is RIDI_DEVICEINFO, set RID_DEVICE_INFO.cbSize to
			//sizeof(RID_DEVICE_INFO) before calling GetRawInputDeviceInfo.
			pData->cbSize = sizeof(RID_DEVICE_INFO);

			//Returns bytes copied to pData, gets input data in pData.
			r = ::GetRawInputDeviceInfo(
				ri_sys_dev[i].hDevice,
				RIDI_DEVICEINFO,
				pData.get(),
				&pcbSize);

			if (r == -1) throw Input_Exception(TEXT("GetRawInputDeviceInfo()"), GetLastError());

			RAWINPUTDEVICE ri_dev = {
				HID_USAGE_PAGE,
				usage_array[pData->dwType],
				0L, //RIDEV_INPUTSINK,
				hwnd
			};

			ri_devices_.resize(ri_devices_.size()+1, ri_dev);
		}
	}

	if (!::RegisterRawInputDevices(
		&ri_devices_[0],
		ri_devices_.size(),
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

RawInput::Input::~Input(void)
{
	for (unsigned int i = 0; i < ri_devices_.size(); ++i)
		ri_devices_[i].dwFlags = RIDEV_REMOVE;

	::RegisterRawInputDevices(
		&ri_devices_[0],
		ri_devices_.size(),
		sizeof(RAWINPUTDEVICE));
}

vector<RAWINPUTDEVICELIST> RawInput::Input::GetSystemDevices(void) const
{
	unsigned int r, ri_system_count;

	//Returns the number of RAWINPUTDEVICELIST structures that can be
	//contained in the buffer to which pRawInputDeviceList points
	//If this value is less than the number of devices attached to the
	//system, the function returns the actual number of devices in this
	//variable and fails with ERROR_INSUFFICIENT_BUFFER.
	r = ::GetRawInputDeviceList(
		0,
		&ri_system_count,
		sizeof(RAWINPUTDEVICELIST));

	if (r == -1) throw Input_Exception(TEXT("GetRawInputDeviceList()"), ::GetLastError());

	vector<RAWINPUTDEVICELIST> ri_sys_dev(ri_system_count);

	//Populates ri_sys_dev with the number of devices attached to the system.
	r = ::GetRawInputDeviceList(
		&ri_sys_dev[0],
		&ri_system_count,
		sizeof(RAWINPUTDEVICELIST));

	if (r == -1) throw Input_Exception(TEXT("GetRawInputDeviceList()"), ::GetLastError());

	return ri_sys_dev;
}

void RawInput::Input::Update(const LPARAM & lParam)
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
			ri_mouse_ = pData.data.mouse;
			break;
		case RIM_TYPEKEYBOARD:
			ri_keyboard_ = pData.data.keyboard;
#ifdef _DEBUG
			wcout << ri_keyboard_.MakeCode
				<< "\t" << ri_keyboard_.Flags
				<< "\t" << ri_keyboard_.VKey
				<< "\t" << ri_keyboard_.Message
				<< "\t" << ri_keyboard_.ExtraInformation << "\n";
#endif
			break;
		case RIM_TYPEHID:
			ri_hid_ = pData.data.hid;
			break;
	}
}

/*LRESULT RawInput::Input::UpdateBuff(const LPARAM & lParam)
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
		NEXTRAWINPUTBLOCK(j); //Does not work on x64
	}

	return ::DefRawInputProc(&pData, raw_data, cbSizeHeader);
}*/

void RawInput::Input::Clean(void)
{
	ri_mouse_	 = RAWMOUSE();
	ri_keyboard_ = RAWKEYBOARD();
	ri_hid_		 = RAWHID();
}

bool RawInput::Input::KeyUp(const unsigned short & button) const
{
	return ri_keyboard_.VKey == button
		&& (ri_keyboard_.Flags == RI_KEY_BREAK || ri_keyboard_.Flags == 3);
}

bool RawInput::Input::KeyDown(const unsigned short & button) const
{
	return ri_keyboard_.VKey == button
		&& (ri_keyboard_.Flags == RI_KEY_MAKE || ri_keyboard_.Flags == 2);
}

bool RawInput::Input::KeyHeld(const unsigned short & button) const
{
	return ri_keyboard_.VKey == button;
}

bool RawInput::Input::MouseEvent(const Mouse::mouse_buttons & button) const
{
	return ri_mouse_.usButtonFlags == button;
}

void RawInput::Input::MousePos(long & x, long & y) const
{
	x = ri_mouse_.lLastX;
	y = ri_mouse_.lLastY;
}