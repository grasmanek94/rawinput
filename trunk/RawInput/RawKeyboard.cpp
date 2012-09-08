#include "RawKeyboard.h"

#ifndef NDEBUG
#include <iostream>

using std::wcout;
#endif

namespace RawInput
{
	RawKeyboard::RawKeyboard(void)
		: m_data()
	{
	}

	RawKeyboard::~RawKeyboard(void)
	{
	}

	void RawKeyboard::Read(const RAWINPUT & ri)
	{
		m_data = ri.data.keyboard;

#ifndef NDEBUG
		wcout << (m_data.Flags & RI_KEY_BREAK ? TEXT("break: ") : TEXT("make: "))
			<< m_data.VKey
			<< TEXT("\n\n");
#endif
	}

	void RawKeyboard::Clean(void)
	{
		m_data = RAWKEYBOARD();
	}

	const RAWKEYBOARD & RawKeyboard::GetData(void) const
	{
		return m_data;
	}

	bool RawKeyboard::KeyUp(unsigned short key) const
	{
		return m_data.VKey == key && m_data.Flags & RI_KEY_BREAK;
	}

	bool RawKeyboard::KeyDown(unsigned short key) const
	{
		return m_data.VKey == key && !(m_data.Flags & RI_KEY_MAKE);
	}

	bool RawKeyboard::KeyHeld(unsigned short key) const
	{
		return m_data.VKey == key;
	}
}
