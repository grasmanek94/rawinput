#include "RawMouse.h"

#ifndef NDEBUG
#include <iostream>

using std::wcout;
#endif

namespace RawInput
{
	RawMouse::RawMouse(void)
		: m_data()
	{
	}

	RawMouse::~RawMouse(void)
	{
	}

	void RawMouse::Read(const RAWINPUT & ri)
	{
		m_data = ri.data.mouse;

#ifndef NDEBUG
		wcout << m_data.usFlags
			//<< TEXT('\t') << m_data.ulButtons
			<< TEXT('\t') << m_data.usButtonFlags
			<< TEXT('\t') << short(m_data.usButtonData)
			<< TEXT('\t') << m_data.ulRawButtons
			<< TEXT('\t') << m_data.lLastX
			<< TEXT('\t') << m_data.lLastY
			//<< TEXT('\t') << m_data.ulExtraInformation
			<< TEXT('\n');
#endif
	}

	void RawMouse::Clean(void)
	{
		m_data = RAWMOUSE();
	}

	const RAWMOUSE & RawMouse::GetData(void) const
	{
		return m_data;
	}

	bool RawMouse::Button(unsigned short button) const
	{
		return (m_data.usButtonFlags & button) != 0;
	}

	void RawMouse::GetPosXY(long * x, long * y) const
	{
		*x = m_data.lLastX;

		*y = m_data.lLastY;
	}

	short RawMouse::GetWheelDelta(void) const
	{
		return m_data.usButtonFlags & RI_MOUSE_WHEEL
			? m_data.usButtonData
			: 0;
	}
}
