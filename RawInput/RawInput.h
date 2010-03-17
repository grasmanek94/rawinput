#pragma once

#include "RawInputAPI.h"
#include "RawDevice.h"

#include <exception>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <memory>

#ifndef NDEBUG
#include <iostream>
#endif

namespace RawInput
{
	class Input_Exception : public std::exception {
		Input_Exception operator=(const Input_Exception &);
	public:
		const HRESULT hresult;

		explicit Input_Exception(const char * const c = nullptr, HRESULT hr = HRESULT())
			: exception(c),
			hresult(hr)
		{
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
				sizeof(RAWINPUTHEADER)) < 0) throw Input_Exception("GetRawInputData()", GetLastError());
		}

		~UnBuffered(void) { }
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

			if (rBuff < 0) throw Input_Exception("GetRawInputBuffer()", GetLastError());

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

		~Buffered(void) { }
	};

	template
	<
		class UpdatePolicy = UnBuffered
	>
	class Input : public UpdatePolicy
	{
		typedef std::tr1::shared_ptr<RawDevice> ptr_rawdev;

		typedef std::map<HANDLE, ptr_rawdev> ri_rawdev_map;

		typedef std::vector<RAWINPUTDEVICE> ri_device_vec;

		typedef std::wstring Str;

		Input(const Input &);
		Input & operator=(const Input &);
	public:
		/*
		 * @param1: handle to the target window. If NULL it follows the keyboard focus.
		 * @param2, 3, 4: flags for mouse, keyboard and hid, respectibly.
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
		//http://www.microsoft.com/whdc/archive/HID_HWID.mspx
		enum usage_page {
			HID_USAGE_PAGE				= 0x01
		};

		enum usage {
			HID_DEVICE_SYSTEM_MOUSE		= 0x02,
			HID_DEVICE_SYSTEM_KEYBOARD	= 0x06,
			HID_DEVICE_SYSTEM_GAME		= 0x04
		};

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
		ptr_rawdev CreateDevice(DWORD);

		ri_device_vec	ri_registered_devices_;

		ri_rawdev_map	ri_devs_;

		RawMouse		* ri_mouse_;
		RawKeyboard		* ri_keyboard_;
		RawHID			* ri_hid_;
	};

	template<class UpdatePolicy>
	Input<UpdatePolicy>::Input(const HWND hwnd, const DWORD mouse_flags, const DWORD keyb_flags, const DWORD hid_flags)
		: ri_registered_devices_(),
		ri_devs_(),
		ri_mouse_(nullptr),
		ri_keyboard_(nullptr),
		ri_hid_(nullptr)
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
			std::wstringstream ss;
			
			ss << e.what() << TEXT(" has thrown an exception.\n\nCode: ") << e.hresult;

			::MessageBox(
				hwnd,
				ss.str().data(),
				TEXT("RawInput"),
				MB_ICONERROR);
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
		std::size_t pcbSize(sizeof(RID_DEVICE_INFO));

		RID_DEVICE_INFO pData;

		// If uiCommand is RIDI_DEVICEINFO, set RID_DEVICE_INFO.cbSize to
		// sizeof(RID_DEVICE_INFO) before calling GetRawInputDeviceInfo.
		pData.cbSize = sizeof(RID_DEVICE_INFO);

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
	typename Input<UpdatePolicy>::ptr_rawdev Input<UpdatePolicy>::CreateDevice(DWORD type)
	{
		ptr_rawdev r;

		switch (type) {
			case RIM_TYPEMOUSE:
				r.reset(ri_mouse_ == nullptr	// if system device is not set
					? ri_mouse_ = new RawMouse	// use the new device as the system device and return it
					: new RawMouse);			// else just return a new device.
				break;
			case RIM_TYPEKEYBOARD:
				r.reset(ri_keyboard_ == nullptr
					? ri_keyboard_ = new RawKeyboard
					: new RawKeyboard);
				break;
			case RIM_TYPEHID:
				r.reset(ri_hid_ == nullptr
					? ri_hid_ = new RawHID
					: new RawHID);
				break;
			default:
				throw Input_Exception("CreateDevice() recieved a bad type.");
		}

		return r;
	}

	template <class UpdatePolicy>
	LRESULT Input<UpdatePolicy>::Update(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		RAWINPUT rData;

		this->UpdatePolicy::Update(reinterpret_cast<HRAWINPUT>(lParam), &rData);

		auto it = ri_devs_.find(rData.header.hDevice);

		if (it != ri_devs_.end()) { // if device found
			it->second.get()->Read(rData); // read data
		}
		else { // else add new device
			auto newdev = ri_devs_[rData.header.hDevice] = this->CreateDevice(rData.header.dwType);

			newdev->Read(rData);
		}

		return ::DefWindowProc(hwnd, message, wParam, lParam);
	}

	template <class UpdatePolicy>
	LRESULT Input<UpdatePolicy>::Change(WPARAM wParam, LPARAM lParam)
	{
		switch (wParam) {
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
		return ri_keyboard_->KeyUp(key);
	}

	template <class UpdatePolicy>
	bool Input<UpdatePolicy>::KeyDown(unsigned short key) const
	{
		return ri_keyboard_->KeyDown(key);
	}

	template <class UpdatePolicy>
	bool Input<UpdatePolicy>::KeyHeld(unsigned short key) const
	{
		return ri_keyboard_->KeyHeld(key);
	}

	template <class UpdatePolicy>
	bool Input<UpdatePolicy>::MouseButtonHeld(unsigned long button) const
	{
		return ri_mouse_->ButtonHeld(button);
	}

	template <class UpdatePolicy>
	bool Input<UpdatePolicy>::MouseButton(unsigned short flag) const
	{
		return ri_mouse_->Button(flag);
	}

	template <class UpdatePolicy>
	void Input<UpdatePolicy>::MousePos(long * x, long * y) const
	{
		ri_mouse_->GetPosXY(x, y);
	}
}