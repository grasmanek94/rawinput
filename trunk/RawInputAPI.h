#pragma once
#pragma warning(disable:4251)

//////////////////////////////////////////////////////////////////////////

// Project http://code.google.com/p/rawinput/
// RawInput http://msdn.microsoft.com/en-us/library/ms645536%28VS.85%29.aspx

//////////////////////////////////////////////////////////////////////////

#ifdef INPUT_EXPORTS
#	define INPUT_API __declspec(dllexport)
#else
#	define INPUT_API __declspec(dllimport)
#endif

//////////////////////////////////////////////////////////////////////////

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef STRICT
#define STRICT
#endif

#include <Windows.h>

namespace Mouse
{
	const enum mouse_rawstate : unsigned long {
		BUTTON_LEFT		= 1L,
		BUTTON_RIGHT	= 1L << 1,
		BUTTON_MIDDLE	= 1L << 2,
		BUTTON_4		= 1L << 3,
		BUTTON_5		= 1L << 4
	};

	const enum mouse_flags : unsigned short {
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

// VKey codes http://msdn.microsoft.com/en-us/library/ms645540%28VS.85%29.aspx
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