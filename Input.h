#pragma once

#pragma warning(disable:4251)

//////////////////////////////////////////////////////////////////////////

#ifdef INPUT_EXPORTS
#	define INPUT_API __declspec(dllexport)
#else
#	define INPUT_API __declspec(dllimport)
#endif

//////////////////////////////////////////////////////////////////////////

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#ifndef STRICT
#define STRICT
#endif

#include <Windows.h>

#include <exception>
#include <string>
#include <vector>

namespace Engin5
{
	// RawInput index http://msdn.microsoft.com/en-us/library/ms645536%28VS.85%29.aspx
	// VKey codes http://msdn.microsoft.com/en-us/library/ms645540%28VS.85%29.aspx

	class INPUT_API Input {
	public:			
		explicit Input(const HWND hwnd = 0);
		~Input(void) throw();

		void Update(const LPARAM & lParam);
		//LRESULT UpdateBuff(const LPARAM & lParam); // Bugged?

		bool KeyUp(const unsigned short & button) const {
			return ri_keyboard_.VKey == button && ri_keyboard_.Flags == RI_KEY_BREAK;
		}

		bool KeyDown(const unsigned short & button) const {
			return ri_keyboard_.VKey == button && ri_keyboard_.Flags == RI_KEY_MAKE;
		}

		bool KeyHeld(const unsigned short & button) const {
			return ri_keyboard_.VKey == button;
		}

		bool MouseEvent(const unsigned short & button) const;
		void MousePos(long & x, long & y) const;

		void Clean(void);

		unsigned int GetRegisteredInputDevicesCount(void);
	private:
		//http://www.microsoft.com/whdc/archive/HID_HWID.mspx
		const static unsigned short HID_USAGE_PAGE				= 0x01;

		const static unsigned short HID_DEVICE_SYSTEM_MOUSE		= 0x02; // also 0x01;
		const static unsigned short HID_DEVICE_SYSTEM_KEYBOARD	= 0x06; // also 0x07;
		const static unsigned short HID_DEVICE_SYSTEM_GAME		= 0x04; // also 0x05;
		//const static USHORT HID_DEVICE_SYSTEM_CONTROL	= 0x80;

		std::vector<RAWINPUTDEVICE> ri_devices_;

		RAWMOUSE		ri_mouse_;
		RAWKEYBOARD		ri_keyboard_;
		RAWHID			ri_hid_;
	};

	namespace Mouse
	{
		const enum mouse_buttons : unsigned short {
			BUTTON0_DOWN	= RI_MOUSE_BUTTON_1_DOWN,
			BUTTON0_UP		= RI_MOUSE_BUTTON_1_UP,

			BUTTON1_DOWN	= RI_MOUSE_BUTTON_2_DOWN,
			BUTTON1_UP		= RI_MOUSE_BUTTON_2_UP,

			BUTTON2_DOWN	= RI_MOUSE_BUTTON_3_DOWN,
			BUTTON2_UP		= RI_MOUSE_BUTTON_3_UP,

			BUTTON3_DOWN	= RI_MOUSE_BUTTON_4_DOWN,
			BUTTON3_UP		= RI_MOUSE_BUTTON_4_UP,

			BUTTON4_DOWN	= RI_MOUSE_BUTTON_5_DOWN,
			BUTTON4_UP		= RI_MOUSE_BUTTON_5_UP,

			WHEEL			= RI_MOUSE_WHEEL
		};
	}

	namespace Keyboard
	{
		const enum keyboard_vkeys : unsigned short {
			VK_0 = 0x30,
			VK_1,
			VK_2,
			VK_3,
			VK_4,
			VK_5,
			VK_6,
			VK_7,
			VK_8,
			VK_9,
			VK_A = 0x41,
			VK_B,
			VK_C,
			VK_D,
			VK_E,
			VK_F,
			VK_G,
			VK_H,
			VK_I,
			VK_J,
			VK_K,
			VK_L,
			VK_M,
			VK_N,
			VK_O,
			VK_P,
			VK_Q,
			VK_R,
			VK_S,
			VK_T,
			VK_U,
			VK_V,
			VK_W,
			VK_X,
			VK_Y,
			VK_Z
		};
	}

	struct Input_Exception : public std::exception {
		const std::wstring error_msg;
		const HRESULT hresult;

		Input_Exception(const std::wstring & c = 0, HRESULT hr = 0) : error_msg(c), hresult(hr) {};
	};
}