#ifndef RawInput_h__
#define RawInput_h__

#include "RawInputAPI.h"

#include "RawMouse.h"
#include "RawKeyboard.h"
#include "RawHID.h"

#include <exception>
#include <algorithm>
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
	class RawInputException : public std::exception {
		RawInputException operator=(const RawInputException &);
	public:
		const HRESULT m_hresult;

		explicit RawInputException(const std::string & msg, HRESULT hr = ::GetLastError())
			: exception(msg.data()),
			m_hresult(hr)
		{
		}

		void Show(void)
		{
			std::wstringstream ss;
			
			ss << this->what() << TEXT(" has thrown an exception.\n\nCode: ") << m_hresult;

			::MessageBox(
				nullptr,
				ss.str().c_str(),
				TEXT("RawInput"),
				MB_ICONERROR);
		}
	};

	class Unbuffered {
		protected:
		/*
		 * UnBuffered Update() implementation.
		 */
		void Update(const HRAWINPUT hrawinput, RAWINPUT * rData)
		{
			std::size_t pcbSize(sizeof(RAWINPUT));

			// Returns -1 on fail. Returns input data in rData.
			if (::GetRawInputData(
				hrawinput,
				RID_INPUT,
				rData,
				&pcbSize,
				sizeof(RAWINPUTHEADER)) == -1) throw RawInputException("GetRawInputData()");
		}
	};

	class Buffered {
		protected:
		/*
		 * WIP
		 *
		 * Buffered Update() implementation.
		 */
		void Update(const HRAWINPUT /*hrawinput*/, RAWINPUT * /*rData*/)
		{
			std::size_t pcbSize;

			// If pData is NULL,
			// the minimum required buffer, in bytes, is returned in pcbSize.
			std::size_t rBuff(::GetRawInputBuffer(
				nullptr,
				&pcbSize,
				sizeof(RAWINPUTHEADER)));

			if (rBuff == -1) throw RawInputException("GetRawInputBuffer()");

			std::vector<RAWINPUT> pData(pcbSize / sizeof(RAWINPUT));

			// If rData is not NULL and the function is successful,
			// the return value is the number of RAWINPUT structures written to pData.
			rBuff = ::GetRawInputBuffer(
				&pData[0],
				&pcbSize,
				sizeof(RAWINPUTHEADER));

			if (rBuff == -1) throw RawInputException("GetRawInputBuffer()");

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
		class BufferingPolicy
	>
	class Input : public BufferingPolicy
	{
		typedef std::shared_ptr<RawDevice> RawDevicePtr;

		typedef std::map<HANDLE, RawDevicePtr> RawdevMap;

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
		explicit Input(const HWND hwnd, const DWORD mouse_flags = 0, const DWORD keyb_flags = 0, const DWORD hid_flags = 0)
			: m_ri_registered_devices(),
			m_ri_devs(),
			m_sys_mouse(),
			m_sys_keyboard(),
			m_sys_hid()
		{
			try {
				const USHORT usage_table[] = {
					HID_DEVICE_SYSTEM_MOUSE,	// RIM_TYPEMOUSE == [0]
					HID_DEVICE_SYSTEM_KEYBOARD,	// RIM_TYPEKEYBOARD == [1]
					HID_DEVICE_SYSTEM_GAME,		// RIM_TYPEHID == [2]
				};

				const DWORD flags_table[] = {
					mouse_flags,				// Flags for mouse
					keyb_flags,					// Flags for keyboard
					hid_flags,					// Flags for HID
				};

				auto ri_sys_dev = EnumSystemDevices();

				std::for_each (
					ri_sys_dev.begin(),
					ri_sys_dev.end(),
					[&](RAWINPUTDEVICELIST & ridl)
					{
#ifndef NDEBUG
						const Str & device_name = this->GetDeviceName(ridl.hDevice);

						std::wcout << device_name << TEXT("\n\n");
#endif

						RID_DEVICE_INFO & dev_info = this->GetDeviceInfo(ridl.hDevice);

						RAWINPUTDEVICE ri_dev = {
							HID_USAGE_PAGE,
							usage_table[dev_info.dwType],
							flags_table[dev_info.dwType],
							hwnd
						};

						m_ri_registered_devices.push_back(ri_dev);

						m_ri_devs[ridl.hDevice] = this->CreateDevice(dev_info.dwType);
					}
				);

				if (!::RegisterRawInputDevices(
					&m_ri_registered_devices[0],
					m_ri_registered_devices.size(),
					sizeof(RAWINPUTDEVICE))) throw RawInputException("RegisterRawInputDevices()");
			}
			catch (RawInputException & ex) {
				ex.Show();
			}
		}

		~Input(void)
		{
			std::for_each (
				m_ri_registered_devices.begin(),
				m_ri_registered_devices.end(),
				[&](RAWINPUTDEVICE & dev)
				{
					dev.dwFlags = RIDEV_REMOVE;

					dev.hwndTarget = nullptr;
				}
			);

			::RegisterRawInputDevices(
				&m_ri_registered_devices[0],
				m_ri_registered_devices.size(),
				sizeof(RAWINPUTDEVICE));
		}


		/*
		 * Update()s devices state. Call on WM_INPUT.
		 */
		LRESULT Update(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			RAWINPUT rData;

			this->BufferingPolicy::Update(reinterpret_cast<HRAWINPUT>(lParam), &rData);

			auto device = m_ri_devs.find(rData.header.hDevice); // look for device

			if (device != m_ri_devs.end()) {	// if device found
				device->second->Read(rData);	// read data
			}
			else {								// else add new device and read data
				auto newdev = this->CreateDevice(rData.header.dwType);

				m_ri_devs[rData.header.hDevice] = newdev;

				newdev->Read(rData);
			}

			switch (rData.header.dwType) {
				case RIM_TYPEMOUSE:
					if (device->second.get() != m_sys_mouse) {
						m_sys_mouse = static_cast<RawMouse *>(&*device->second);
					}
					break;
				case RIM_TYPEKEYBOARD:
					if (device->second.get() != m_sys_keyboard) {
						m_sys_keyboard = static_cast<RawKeyboard *>(&*device->second);
					}
					break;
				case RIM_TYPEHID:
				default:
					;
			}

			switch (GET_RAWINPUT_CODE_WPARAM(wParam)) {
				case RIM_INPUT: // input ocurred when the window was in the foreground.
				case RIM_INPUTSINK: // input ocurred when the window was not in the foreground.
					return ::DefWindowProc(hwnd, message, wParam, lParam);
				default:
					return 0;
			}
		}

		/*
		 * Call Change() to handle WM_INPUT_DEVICE_CHANGE messages
		 */
		LRESULT Change(WPARAM wParam, LPARAM lParam)
		{
			switch (GET_RAWINPUT_CODE_WPARAM(wParam)) {
			case GIDC_ARRIVAL:
				// Handle arrival

				#ifndef NDEBUG
				std::wcout << TEXT("RawDevice arrival detected: ") << this->GetDeviceName(reinterpret_cast<HANDLE>(lParam));
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

		/*
		 * Clean()s the state for all devices.
		 */
		void Clean(void)
		{
			std::for_each(
				m_ri_devs.begin(),
				m_ri_devs.end(),
				[&](RawdevMap::iterator::value_type & dev)
				{
					dev.second->Clean();
				}
			);
		}

		bool KeyUp(unsigned short key) const { return m_sys_keyboard->KeyUp(key); }

		bool KeyDown(unsigned short key) const { return m_sys_keyboard->KeyDown(key); }

		bool KeyHeld(unsigned short key) const { return m_sys_keyboard->KeyHeld(key); }

		bool MouseButton(unsigned short button) const { return m_sys_mouse->Button(button); }

		void MousePos(long * x, long * y) const { m_sys_mouse->GetPosXY(x, y); }
	private:
		/*
		 * EnumSystemDevices() returns a vector with all the devices
		 * connected to the system at the time of call.
		 */
		static std::vector<RAWINPUTDEVICELIST> EnumSystemDevices(void)
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
				sizeof(RAWINPUTDEVICELIST)) < 0) throw RawInputException("GetRawInputDeviceList()");

			std::vector<RAWINPUTDEVICELIST> ri_sys_dev(ri_system_count);

			//Populates ri_sys_dev with the number of devices attached to the system.
			if (::GetRawInputDeviceList(
				&ri_sys_dev[0],
				&ri_system_count,
				sizeof(RAWINPUTDEVICELIST)) < 0) throw RawInputException("GetRawInputDeviceList()");

			return std::vector<RAWINPUTDEVICELIST>(ri_sys_dev);
		}

		/*
		 * GetDeviceInfo() returns a RID_DEVICE_INFO filled with a device.
		 *
		 * @param1: HANDLE to a device
		 */
		RID_DEVICE_INFO GetDeviceInfo(HANDLE hDevice) const
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
				&pcbSize) < 0) throw RawInputException("GetRawInputDeviceInfo(RIDI_DEVICEINFO)");

			return pData;
		}

		/*
		 * GetDeviceName() returns device name.
		 *
		 * @param1: HANDLE to a device
		 */
		Str GetDeviceName(HANDLE hDevice) const
		{
			std::size_t pcbSize;

			//If successful, this function returns a non-negative number indicating the number of character copied to pData.
			//If pData is NULL, the function returns a value of zero.
			//If pData is not large enough for the data, the function returns -1. 
			if (::GetRawInputDeviceInfo(
				hDevice,
				RIDI_DEVICENAME,
				nullptr,
				&pcbSize) < 0) throw RawInputException("GetRawInputDeviceInfo(RIDI_DEVICENAME)");

			std::vector<Str::value_type> pData(pcbSize);

			if (::GetRawInputDeviceInfo(
				hDevice,
				RIDI_DEVICENAME,
				&pData[0],
				&pcbSize) < 0) throw RawInputException("GetRawInputDeviceInfo(RIDI_DEVICENAME)");

			return &pData[0]; // constructs Str from vector
		}

		/*
		 * CreateDevice() returns a pointer to a new device.
		 *
		 * @param1: DWORD corresponding to a device type
		 */
		RawDevicePtr CreateDevice(DWORD type)
		{
			switch (type) {
			case RIM_TYPEMOUSE:
				return RawDevicePtr(m_sys_mouse == nullptr	// if system device is not set
					? m_sys_mouse = new RawMouse			// use the new device as the system device and return it
					: new RawMouse);						// else just create a new device.
			case RIM_TYPEKEYBOARD:
				return RawDevicePtr(m_sys_keyboard == nullptr
					? m_sys_keyboard = new RawKeyboard
					: new RawKeyboard);
			case RIM_TYPEHID:
				return RawDevicePtr(m_sys_hid == nullptr
					? m_sys_hid = new RawHID
					: new RawHID);
			default:
				throw RawInputException("CreateDevice() recieved a bad type.");
			}
		}

	private:
		DeviceVec	m_ri_registered_devices;

		RawdevMap	m_ri_devs;

		RawMouse	* m_sys_mouse;

		RawKeyboard	* m_sys_keyboard;

		RawHID		* m_sys_hid;
	};
}

#endif // RawInput_h__
