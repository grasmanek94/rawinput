//////////////////////////////////////////////////////////////////////////

// Project http://rawinput.googlecode.com

// MSDN RawInput http://msdn.microsoft.com/en-us/library/ms645536%28VS.85%29.aspx

//////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef RawInputAPI_h__
#define RawInputAPI_h__


#ifdef RAWINPUT_EXPORTS
#define RAWINPUT_API __declspec(dllexport)
#else
#define RAWINPUT_API __declspec(dllimport)
#endif


#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef STRICT
#define STRICT
#endif

#pragma warning(push, 0)
#include <Windows.h>
#pragma warning(pop)

#endif // RawInputAPI_h__
