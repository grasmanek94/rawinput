#pragma once

#include "RawDevice.h"
#include "RawInputAPI.h"

#include <exception>
#include <string>
#include <vector>

namespace RawInput
{
	class INPUT_API Input {
		Input(const Input &);
		Input & operator=(const Input &);
	public:			
		explicit Input(const HWND hwnd = 0);
		~Input(void) throw();

		/*
		 * Basic Update() method.
		 *
		 * @param1: LPARAM from main window's WndProc()
		 */
		void Update(const LPARAM & lParam);

		/*
		 * Buffered Update() method, needs work. Pre strict c++
		 *
		 * @param1: LPARAM from main window's WndProc()
		 */
		//LRESULT UpdateBuff(const LPARAM & lParam); // Bugged?

		/*
		 * Clean()s the state for all devices. Needs to be called at the end of each loop.
		 */
		void Clean(void);

		/*
		 * KeyUp() returns true on key release.
		 *
		 * @param1: one of Keyboard::keyboard_vkeys enum or VK code.
		 */
		bool KeyUp(const unsigned short & button) const;

		/*
		 * KeyDown() returns true on key press.
		 *
		 * @param1: one of Keyboard::keyboard_vkeys enum or VK code.
		 */
		bool KeyDown(const unsigned short & button) const;

		/*
		 * KeyHeld() returns true while key is pressed.
		 *
		 * @param1: one of Keyboard::keyboard_vkeys enum or VK code.
		 */
		bool KeyHeld(const unsigned short & button) const;

		/*
		 * MouseEvent() returns true on button press.
		 *
		 * @param1: one of Mouse::mouse_buttons enum.
		 */
		bool MouseEvent(const Mouse::mouse_buttons & button) const;

		/*
		 * MousePos() returns mouse's x/y position.
		 *
		 * @param1: returns mouse position X.
		 * @param2: returns mouse position Y.
		 */
		void MousePos(long & x, long & y) const;
	private:
		/*
		 * GetSystemDevices() returns a vector with all the devices
		 * connected to the system at the time of call.
		 */
		std::vector<RAWINPUTDEVICELIST> GetSystemDevices(void) const;

		//http://www.microsoft.com/whdc/archive/HID_HWID.mspx
		const static unsigned short HID_USAGE_PAGE				= 0x01;

		const static unsigned short HID_DEVICE_SYSTEM_MOUSE		= 0x02; // also 0x01;
		const static unsigned short HID_DEVICE_SYSTEM_KEYBOARD	= 0x06; // also 0x07;
		const static unsigned short HID_DEVICE_SYSTEM_GAME		= 0x04; // also 0x05;
		//const static unsigned short HID_DEVICE_SYSTEM_CONTROL	= 0x80;

		std::vector<RAWINPUTDEVICE> ri_devices_;

		RAWMOUSE	ri_mouse_;
		RAWKEYBOARD	ri_keyboard_;
		RAWHID		ri_hid_;
	};

	struct Input_Exception : public std::exception {
		const std::wstring error_msg;
		const HRESULT hresult;

		Input_Exception(const std::wstring & c = 0, HRESULT hr = 0) : error_msg(c), hresult(hr) {};
	};
}