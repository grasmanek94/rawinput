#include "RawInputAPI.h"

#include "RawHID.h"

#ifndef NDEBUG
#include <iostream>

using std::wcout;
#endif

namespace RawInput
{
	RawHID::RawHID(void)
		: m_data()
	{
	}

	RawHID::~RawHID(void)
	{
	}

	void RawHID::Read(const RAWINPUT & ri)
	{
		m_data = ri.data.hid;

#ifndef NDEBUG
		DWORD size(m_data.dwSizeHid * m_data.dwCount - 1);

		for (DWORD i = 0; i < size; ++i) wcout << std::hex << m_data.bRawData[i] << std::dec << TEXT('\t');

		wcout << TEXT('\n');
#endif
	}

	const RAWHID & RawHID::GetData(void) const
	{
		return m_data;
	}
}