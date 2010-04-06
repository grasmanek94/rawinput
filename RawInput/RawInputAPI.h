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