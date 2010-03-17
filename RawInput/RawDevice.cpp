#include "RawDevice.h"

#ifndef NDEBUG
#include <iostream>

using std::wcout;
#endif

namespace RawInput
{
	RawDevice::~RawDevice(void)
	{
	}

	RawMouse::~RawMouse(void)
	{
	}

	const RAWINPUT & RawMouse::Read(const RAWINPUT & ri)
	{
		ri_data_ = ri.data.mouse;

		#ifndef NDEBUG
		wcout << ri_data_.usFlags
//			<< TEXT('\t') << ri_data_.ulButtons
			<< TEXT('\t') << ri_data_.usButtonFlags
			<< TEXT('\t') << short(ri_data_.usButtonData)
			<< TEXT('\t') << ri_data_.ulRawButtons
			<< TEXT('\t') << ri_data_.lLastX
			<< TEXT('\t') << ri_data_.lLastY
//			<< TEXT('\t') << ri_data_.ulExtraInformation
			<< TEXT('\n');
		#endif
			
		return ri;
	}

	void RawMouse::Clean(void)
	{
		ri_data_ = RAWMOUSE();
	}

	const RAWMOUSE & RawMouse::GetData(void)
	{
		return ri_data_;
	}

	bool RawMouse::Button(unsigned short button) const
	{
		return ri_data_.usButtonFlags == button;
	}

	bool RawMouse::ButtonHeld(unsigned short button) const
	{
		return ri_data_.ulRawButtons == button;
	}

	void RawMouse::GetPosXY(long * x, long * y) const
	{
		*x = ri_data_.lLastX;
		*y = ri_data_.lLastY;
	}

	short RawMouse::GetWheelDelta(void) const
	{
		return ri_data_.usButtonData;
	}

	RawKeyboard::~RawKeyboard(void)
	{
	}

	const RAWINPUT & RawKeyboard::Read(const RAWINPUT & ri)
	{
		ri_data_ = ri.data.keyboard;

		#ifndef NDEBUG
		wcout << ri_data_.MakeCode
			<< TEXT('\t') << ri_data_.Flags
			<< TEXT('\t') << ri_data_.VKey
			<< TEXT('\t') << ri_data_.Message
//			<< TEXT('\t') << ri_data_.ExtraInformation
			<< TEXT('\n');
		#endif
			
		return ri;
	}

	void RawKeyboard::Clean(void)
	{
		ri_data_ = RAWKEYBOARD();
	}

	const RAWKEYBOARD & RawKeyboard::GetData(void)
	{
		return ri_data_;
	}

	bool RawKeyboard::KeyUp(unsigned short key) const
	{
		return (
				ri_data_.VKey == key
			) && (
				ri_data_.Flags == RI_KEY_BREAK ||
				ri_data_.Flags == RI_KEY_BREAK + RI_KEY_E0
			);
	}

	bool RawKeyboard::KeyDown(unsigned short key) const
	{
		return (
				ri_data_.VKey == key
			) && (
				ri_data_.Flags == RI_KEY_MAKE ||
				ri_data_.Flags == RI_KEY_MAKE + RI_KEY_E0
			);
	}

	bool RawKeyboard::KeyHeld(unsigned short key) const
	{
		return (
				ri_data_.VKey == key
			) && (
				ri_data_.Flags != RI_KEY_BREAK ||
				ri_data_.Flags != RI_KEY_BREAK + RI_KEY_E0
			);
	}

	RawHID::~RawHID(void)
	{
	}

	const RAWINPUT & RawHID::Read(const RAWINPUT & ri)
	{
		ri_data_ = ri.data.hid;

		#ifndef NDEBUG
		int tot(ri_data_.dwSizeHid * ri_data_.dwCount);

		for (int i = 0; i < tot - 1; ++i) wcout << ri_data_.bRawData[i] << TEXT('\t');

		wcout << TEXT('\n');
		#endif
			
		return ri;
	}

	void RawHID::Clean(void)
	{
		ri_data_ = RAWHID();
	}

	const RAWHID & RawHID::GetData(void)
	{
		return ri_data_;
	}
}