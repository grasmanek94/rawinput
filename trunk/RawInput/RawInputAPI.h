//////////////////////////////////////////////////////////////////////////

// Project http://code.google.com/p/rawinput/

// MSDN RawInput http://msdn.microsoft.com/en-us/library/ms645536%28VS.85%29.aspx

//////////////////////////////////////////////////////////////////////////

#ifndef RawInputAPI_h__
#define RawInputAPI_h__

#pragma once

#include "stdafx.h"

#ifdef RAWINPUT_EXPORTS
#define RAWINPUT_API __declspec(dllexport)
#else
#define RAWINPUT_API __declspec(dllimport)
#endif

#endif // RawInputAPI_h__