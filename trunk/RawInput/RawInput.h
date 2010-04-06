#pragma once

#include "RawInputAPI.h"

#include "RawDevice.h"

#include <exception>

#include <map>

#include <vector>

#include <memory>

#include <string>

#include <sstream>

#ifndef NDEBUG
#include <iostream>
#endif

namespace RawInput
{
	class Input_Exception : public std::exception {
		Input_Exception operator=(const Input_Exception &);
	public:
		const HRESULT hresult_;

		explicit Input_Exception(const std::string msg, HRESULT hr = HRESULT())
			: exception(msg.data()),
			hresult_(hr)
		{
		}

		void Show(void)
		{
			std::wstringstream ss;
			
			ss << this->what() << TEXT(" has thrown an exception.\n\nCode: ") << hresult_;

			::MessageBox(
				nullptr,
				ss.str().data(),
				TEXT("RawInput"),
				MB_ICONERROR);
		}
	};

	class UnBuffered {
		protected:
		/*
		 * UnBuffered Update() implementation.
		 */
		void Update(const HRAWINPUT hrawinput, RAWINPUT * rData)
		{
			std::size_t pcbSize(sizeof(RAWINPUT));

			// Returns -1 on fail. Returns input data in rData.
			if(::GetRawInputData(
				hrawinput,
				RID_INPUT,
				rData,
				&pcbSize,
				sizeof(RAWINPUTHEADER)) == -1) throw Input_Exception("GetRawInputData()", GetLastError());
		}
	};

	class Buffered {
		protected:
		/*
		 * WIP
		 *
		 * Buffered Update() implementation.
		 */
		void Update(const HRAWINPUT hrawinput, RAWINPUT * rData)
		{
			std::size_t pcbSize;

			// If pData is NULL,
			// the minimum required buffer, in bytes, is returned in pcbSize.
			std::size_t rBuff(::GetRawInputBuffer(
				nullptr,
				&pcbSize,
				sizeof(RAWINPUTHEADER)));

			if (rBuff == -1) throw Input_Exception("GetRawInputBuffer()", GetLastError());

			std::vector<RAWINPUT> pData(pcbSize / sizeof(RAWINPUT));

			// If rData is not NULL and the function is successful,
			// the return value is the number of RAWINPUT structures written to pData.
			rBuff = ::GetRawInputBuffer(
				&pData[0],
				&pcbSize,
				sizeof(RAWINPUTHEADER));

			if (rBuff == -1) throw Input_Exception("GetRawInputBuffer()", GetLastError());

			if (rBuff == 0) return;

			/*for (std::size_t i = 0; i < rBuff; ++i) {
				switch (pData[i].header.dwType) {
					case RIM_TYPEMOUSE:
						ri_mouse_ << pData[i];
						break;
					case RIM_TYPEKEYBOARD:
						ri_keyboard_ << pData[i];
						break;
					case RIM_TYPEHID:
						ri_hid_ << pData[i];
						break;
				}
			}*/
		}
	};

	template
	<
		class UpdatePolicy = UnBuffered
	>
	class Input : public UpdatePolicy
	{
		typedef std::tr1::shared_ptr<RawDevice> PtrRawdev;

		typedef std::map<HANDLE, PtrRawdev> RawdevMap;

		typedef std::vector<RAWINPUTDEVICE> DeviceVec;

		typedef std::wstring Str;

		//http://www.microsoft.com/whdc/archive/HID_HWID.mspx
		enum usage_page {
			HID_USAGE_PAGE				= 0x01
		};

		enum usage {
			HID_DEVICE_SYSTEM_MOUSE		= 0x02,
			HID_DEVICE_SYSTEM_KEYBOARD	= 0x06,
			HID_DEVICE_SYSTEM_GAME		= 0x04
		};

		Input(const Input &);

		Input & operator=(const Input &);
	public:
		/*
		 * @param1: handle to the target window. If NULL it follows the keyboard focus.
		 * @param2, 3, 4: flags for mouse, keyboard and hid, respectively.
		 */
		explicit Input(const HWND, const DWORD mouse_flags = 0, const DWORD keyb_flags = 0, const DWORD hid_flags = 0);

		~Input(void);

		/*
		 * Update()s devices state. Call on WM_INPUT.
		 */
		LRESULT Update(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

		/*
		 * Call Change() to handle WM_INPUT_DEVICE_CHANGE messages
		 */
		LRESULT Change(WPARAM, LPARAM);

		/*
		 * Clean()s the state for all devices.
		 */
		void Clean(void);

		/*
		 * KeyUp() returns true on key release.
		 *
		 * @param1: one of Keyboard::keyboard_vkeys enum or VK code.
		 */
		bool KeyUp(unsigned short) const;

		/*
		 * KeyDown() returns true on key press.
		 *
		 * @param1: one of Keyboard::keyboard_vkeys enum or VK code.
		 */
		bool KeyDown(unsigned short) const;

		/*
		 * KeyHeld() returns true while key is pressed.
		 *
		 * @param1: one of Keyboard::keyboard_vkeys enum or VK code.
		 */
		bool KeyHeld(unsigned short) const;

		/**
		 * !!! Only works when connected through PS/2 port !!!
		 *
		 * MouseButtonHeld() returns true while button is pressed.
		 *
		 * @param1: any combination of Mouse::mouse_rawstate enum.
		 */
		bool MouseButtonHeld(unsigned long) const;

		/*
		 * MouseEvent() returns true on button event.
		 *
		 * @param1: one of Mouse::mouse_flags enum.
		 */
		bool MouseButton(unsigned short) const;

		/*
		 * MousePos() returns mouse's x/y position.
		 *
		 * @param1: returns mouse last position X.
		 * @param2: returns mouse last position Y.
		 */
		void MousePos(long *, long *) const;
	private:
		/*
		 * GetSystemDevices() returns a vector with all the devices
		 * connected to the system at the time of call.
		 */
		std::vector<RAWINPUTDEVICELIST> GetSystemDevices(void) const;

		/*
		 * GetDeviceInfo() returns a RID_DEVICE_INFO filled with a device.
		 *
		 * @param1: HANDLE to a device
		 */
		RID_DEVICE_INFO GetDeviceInfo(HANDLE) const;

		/*
		 * GetDeviceName() returns device name.
		 *
		 * @param1: HANDLE to a device
		 */
		Str GetDeviceName(HANDLE) const;

		/*
		 * CreateDevice() returns a pointer to a new device.
		 *
		 * @param1: DWORD corresponding to a device type
		 */
		PtrRawdev CreateDevice(DWORD);

		DeviceVec	ri_registered_devices_;

		RawdevMap	ri_devs_;

		RawMouse	* sys_mouse_;

		RawKeyboard	* sys_keyboard_;

		RawHID		* sys_hid_;
	};

	template<class UpdatePolicy>
	Input<UpdatePolicy>::Input(const HWND hwnd, const DWORD mouse_flags, const DWORD keyb_flags, const DWORD hid_flags)
		: ri_registered_devices_(),
		ri_devs_(),
		sys_mouse_(),
		sys_keyboard_(),
		sys_hid_()
	{
		try {
			auto & ri_sys_dev(this->GetSystemDevices());

			const USHORT usage_table[] = {
				HID_DEVICE_SYSTEM_MOUSE,	// RIM_TYPEMOUSE == 0
				HID_DEVICE_SYSTEM_KEYBOARD,	// RIM_TYPEKEYBOARD == 1
				HID_DEVICE_SYSTEM_GAME,		// RIM_TYPEHID == 2
			};

			const DWORD flags_table[] = {
				mouse_flags,				// Flags for mouse
				keyb_flags,					// Flags for keyboard
				hid_flags,					// Flags for HID
			};

			for (std::size_t i = 0; i < ri_sys_dev.size(); ++i) {
				Str & device_name(this->GetDeviceName(ri_sys_dev[i].hDevice));

				if (device_name.find(TEXT("RDP_")) == -1) { //Look for non-RDP devices
					#ifndef NDEBUG
					std::wcout << TEXT("Device[") << i << TEXT("]: ") << device_name << TEXT("\n\n");
					#endif

					RID_DEVICE_INFO & pData(this->GetDeviceInfo(ri_sys_dev[i].hDevice));
					
					RAWINPUTDEVICE ri_dev = {
						HID_USAGE_PAGE,
						usage_table[pData.dwType],
						flags_table[pData.dwType],
						hwnd
					};

					ri_registered_devices_.push_back(ri_dev);

					ri_devs_[ri_sys_dev[i].hDevice] = this->CreateDevice(pData.dwType);
				}
			}

			if (!::RegisterRawInputDevices(
				&ri_registered_devices_[0],
				ri_registered_devices_.size(),
				sizeof(RAWINPUTDEVICE))) throw Input_Exception("RegisterRawInputDevices()", GetLastError());
		}
		catch (Input_Exception & e) {
			e.Show();
		}
	}

	template<class UpdatePolicy>
	Input<UpdatePolicy>::~Input(void)
	{
		for (auto i = ri_registered_devices_.begin(), end = ri_registered_devices_.end()
			; i != end
			; ++i)
		{
			i->dwFlags = RIDEV_REMOVE;
			i->hwndTarget = nullptr;
		}

		::RegisterRawInputDevices(
			&ri_registered_devices_[0],
			ri_registered_devices_.size(),
			sizeof(RAWINPUTDEVICE));
	}

	template<class UpdatePolicy>
	std::vector<RAWINPUTDEVICELIST> Input<UpdatePolicy>::GetSystemDevices(void) const
	{
		std::size_t ri_system_count;

		//Returns the number of RAWINPUTDEVICELIST structures that can be
		//contained in the buffer to which pRawInputDeviceList points
		//If this value is less than the number of devices attached to the
		//system, the function returns the actual number of devices in this
		//variable and fails with ERROR_INSUFFICIENT_BUFFER.
		if (::GetRawInputDeviceList(
			nullptr,
			&ri_system_count,
			sizeof(RAWINPUTDEVICELIST)) < 0) throw Input_Exception("GetRawInputDeviceList()", ::GetLastError());

		std::vector<RAWINPUTDEVICELIST> ri_sys_dev(ri_system_count);

		//Populates ri_sys_dev with the number of devices attached to the system.
		if (::GetRawInputDeviceList(
			&ri_sys_dev[0],
			&ri_system_count,
			sizeof(RAWINPUTDEVICELIST)) < 0) throw Input_Exception("GetRawInputDeviceList()", ::GetLastError());

		return ri_sys_dev;
	}

	template <class UpdatePolicy>
	RID_DEVICE_INFO Input<UpdatePolicy>::GetDeviceInfo(HANDLE hDevice) const
	{
		RID_DEVICE_INFO pData;

		// If uiCommand is RIDI_DEVICEINFO, set RID_DEVICE_INFO.cbSize to
		// sizeof(RID_DEVICE_INFO) before calling GetRawInputDeviceInfo.
		pData.cbSize = sizeof(RID_DEVICE_INFO);

		std::size_t pcbSize = sizeof(RID_DEVICE_INFO);

		// Returns bytes copied to pData, gets input data in pData.
		if (::GetRawInputDeviceInfo(
			hDevice,
			RIDI_DEVICEINFO,
			&pData,
			&pcbSize) < 0) throw Input_Exception("GetRawInputDeviceInfo(RIDI_DEVICEINFO)", ::GetLastError());

		return pData;
	}

	template <class UpdatePolicy>
	typename Input<UpdatePolicy>::Str Input<UpdatePolicy>::GetDeviceName(HANDLE hDevice) const
	{
		std::size_t pcbSize;

		//If successful, this function returns a non-negative number indicating the number of character copied to pData.
		//If pData is NULL, the function returns a value of zero.
		//If pData is not large enough for the data, the function returns -1. 
		if (::GetRawInputDeviceInfo(
			hDevice,
			RIDI_DEVICENAME,
			nullptr,
			&pcbSize) < 0) throw Input_Exception("GetRawInputDeviceInfo(RIDI_DEVICENAME)", ::GetLastError());

		std::vector<wchar_t> pData(pcbSize);

		if (::GetRawInputDeviceInfo(
			hDevice,
			RIDI_DEVICENAME,
			&pData[0],
			&pcbSize) < 0) throw Input_Exception("GetRawInputDeviceInfo(RIDI_DEVICENAME)", ::GetLastError());

		return &pData[0]; // constructs string from vector
	}

	template <class UpdatePolicy>
	typename Input<UpdatePolicy>::PtrRawdev Input<UpdatePolicy>::CreateDevice(DWORD type)
	{
		switch (type) {
			case RIM_TYPEMOUSE:
				return PtrRawdev(sys_mouse_ == nullptr	// if system device is not set
					? sys_mouse_ = new RawMouse			// use the new device as the system device and return it
					: new RawMouse);					// else just create a new device.
				break;
			case RIM_TYPEKEYBOARD:
				return PtrRawdev(sys_keyboard_ == nullptr
					? sys_keyboard_ = new RawKeyboard
					: new RawKeyboard);
				break;
			case RIM_TYPEHID:
				return PtrRawdev(sys_hid_ == nullptr
					? sys_hid_ = new RawHID
					: new RawHID);
				break;
			default:
				throw Input_Exception("CreateDevice() recieved a bad type.");
		}
	}

	template <class UpdatePolicy>
	LRESULT Input<UpdatePolicy>::Update(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		RAWINPUT rData;

		this->UpdatePolicy::Update(reinterpret_cast<HRAWINPUT>(lParam), &rData);

		auto it = ri_devs_.find(rData.header.hDevice); // look for device

		if (it != ri_devs_.end()) {		// if device found
			it->second->Read(rData);	// read data
		}
		else {							// else add new device and read data
			auto newdev = ri_devs_[rData.header.hDevice] = this->CreateDevice(rData.header.dwType);

			newdev->Read(rData);
		}

		return GET_RAWINPUT_CODE_WPARAM(wParam) == RIM_INPUT
			? ::DefWindowProc(hwnd, message, wParam, lParam)
			: 0;
	}

	template <class UpdatePolicy>
	LRESULT Input<UpdatePolicy>::Change(WPARAM wParam, LPARAM lParam)
	{
		switch (GET_RAWINPUT_CODE_WPARAM(wParam)) {
			case GIDC_ARRIVAL:
				// Handle arrival

				#ifndef NDEBUG
				std::wcout << TEXT("RawDevice arrival: ") << this->GetDeviceName(reinterpret_cast<HANDLE>(lParam));
				#endif

				break;
			case GIDC_REMOVAL:
				// Handle removal

				#ifndef NDEBUG
				std::wcout << TEXT("RawDevice removal detected: ") << this->GetDeviceName(reinterpret_cast<HANDLE>(lParam));
				#endif

				break;
		}

		return 0;
	}

	template <class UpdatePolicy>
	void Input<UpdatePolicy>::Clean(void)
	{
		for (auto it = ri_devs_.begin(); it != ri_devs_.end(); ++it) {
			it->second.get()->Clean();
		}
	}

	template <class UpdatePolicy>
	bool Input<UpdatePolicy>::KeyUp(unsigned short key) const
	{
		return sys_keyboard_->KeyUp(key);
	}

	template <class UpdatePolicy>
	bool Input<UpdatePolicy>::KeyDown(unsigned short key) const
	{
		return sys_keyboard_->KeyDown(key);
	}

	template <class UpdatePolicy>
	bool Input<UpdatePolicy>::KeyHeld(unsigned short key) const
	{
		return sys_keyboard_->KeyHeld(key);
	}

	template <class UpdatePolicy>
	bool Input<UpdatePolicy>::MouseButtonHeld(unsigned long button) const
	{
		return sys_mouse_->ButtonHeld(button);
	}

	template <class UpdatePolicy>
	bool Input<UpdatePolicy>::MouseButton(unsigned short flag) const
	{
		return sys_mouse_->Button(flag);
	}

	template <class UpdatePolicy>
	void Input<UpdatePolicy>::MousePos(long * x, long * y) const
	{
		sys_mouse_->GetPosXY(x, y);
	}
}