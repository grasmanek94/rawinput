//////////////////////////////////////////////////////////////////////////

// Project http://code.google.com/p/rawinput/

// MSDN RawInput http://msdn.microsoft.com/en-us/library/ms645536%28VS.85%29.aspx

//////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////

#ifdef RAWINPUT_EXPORTS
#define RAWINPUT_API __declspec(dllexport)
#else
#define RAWINPUT_API __declspec(dllimport)
#endif

//////////////////////////////////////////////////////////////////////////

namespace Mouse
{
	const enum mouse_rawstate {
		BUTTON_0		= 1,
		BUTTON_1		= 1 << 1,
		BUTTON_2		= 1 << 2,
		BUTTON_3		= 1 << 3,
		BUTTON_4		= 1 << 4
	};

	const enum mouse_flags {
		BUTTON_0_DOWN	= RI_MOUSE_BUTTON_1_DOWN,
		BUTTON_0_UP		= RI_MOUSE_BUTTON_1_UP,

		BUTTON_1_DOWN	= RI_MOUSE_BUTTON_2_DOWN,
		BUTTON_1_UP		= RI_MOUSE_BUTTON_2_UP,

		BUTTON_2_DOWN	= RI_MOUSE_BUTTON_3_DOWN,
		BUTTON_2_UP		= RI_MOUSE_BUTTON_3_UP,

		BUTTON_3_DOWN	= RI_MOUSE_BUTTON_4_DOWN,
		BUTTON_3_UP		= RI_MOUSE_BUTTON_4_UP,

		BUTTON_4_DOWN	= RI_MOUSE_BUTTON_5_DOWN,
		BUTTON_4_UP		= RI_MOUSE_BUTTON_5_UP,

		WHEEL			= RI_MOUSE_WHEEL
	};
}

// VKey codes http://msdn.microsoft.com/en-us/library/ms645540%28VS.85%29.aspx
namespace Keyboard
{
	const enum keyboard_vkeys {
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