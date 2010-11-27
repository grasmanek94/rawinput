/*#include "RawDevice.h"

#ifndef NDEBUG
#include <iostream>

using std::wcout;
#endif

namespace RawInput
{
	RawMouse::RawMouse(void)
		: ri_data_()
	{
	}

	RawMouse::~RawMouse(void)
	{
	}

	void RawMouse::Read(const RAWINPUT & ri)
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
	}

	void RawMouse::Clean(void)
	{
		ri_data_ = RAWMOUSE();
	}

	const RAWMOUSE & RawMouse::GetData(void) const
	{
		return ri_data_;
	}

	bool RawMouse::Button(unsigned short button) const
	{
		return (ri_data_.usButtonFlags & button) != 0;
	}

	void RawMouse::GetPosXY(long * x, long * y) const
	{
		*x = ri_data_.lLastX;

		*y = ri_data_.lLastY;
	}

	short RawMouse::GetWheelDelta(void) const
	{
		return ri_data_.usButtonFlags & RI_MOUSE_WHEEL
			? ri_data_.usButtonData
			: 0;
	}

	RawKeyboard::RawKeyboard(void)
		: ri_data_()
	{
	}

	RawKeyboard::~RawKeyboard(void)
	{
	}

	void RawKeyboard::Read(const RAWINPUT & ri)
	{
		ri_data_ = ri.data.keyboard;

#ifndef NDEBUG
		wcout << (ri_data_.Flags & RI_KEY_BREAK ? TEXT("break: ") : TEXT("make: "))
			<< ri_data_.VKey
			<< TEXT("\n\n");
#endif
	}

	void RawKeyboard::Clean(void)
	{
		ri_data_ = RAWKEYBOARD();
	}

	const RAWKEYBOARD & RawKeyboard::GetData(void) const
	{
		return ri_data_;
	}

	bool RawKeyboard::KeyUp(unsigned short key) const
	{
		return ri_data_.VKey == key && ri_data_.Flags & RI_KEY_BREAK;
	}

	bool RawKeyboard::KeyDown(unsigned short key) const
	{
		return ri_data_.VKey == key && !(ri_data_.Flags & RI_KEY_MAKE);
	}

	bool RawKeyboard::KeyHeld(unsigned short key) const
	{
		return ri_data_.VKey == key;
	}

	RawHID::RawHID(void)
		: ri_data_()
	{
	}

	RawHID::~RawHID(void)
	{
	}

	void RawHID::Read(const RAWINPUT & ri)
	{
		ri_data_ = ri.data.hid;

		#ifndef NDEBUG
		DWORD tot(ri_data_.dwSizeHid * ri_data_.dwCount);

		for (DWORD i = 0; i < tot - 1; ++i) wcout << ri_data_.bRawData[i] << TEXT('\t');

		wcout << TEXT('\n');
		#endif
	}

	void RawHID::Clean(void)
	{
		ri_data_ = RAWHID();
	}

	const RAWHID & RawHID::GetData(void) const
	{
		return ri_data_;
	}
}*/