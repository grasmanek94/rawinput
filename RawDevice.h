#pragma once

#include "RawInputAPI.h"

#include <sstream>

namespace RawInput
{
	template<typename T> class Device {
	public:
		Device(const T & dev = T()) : ri_data_(dev), str_data_() {}
		Device(const Device & other) : ri_data_(other.ri_data_), str_data_() {}
		Device & operator=(const Device & other) { ri_data_ = other.ri_data_; return *this; }
		~Device(void) {}

		Device & operator<<(const RAWINPUT & ri) { return *this; }

		void Clean() { ri_data_ = T(); }

		std::wstring & data_str() { return str_data_; }
	private:
		T ri_data_;

		std::wstring str_data_;

		friend class Input;
	};

	template<> Device<RAWMOUSE> & Device<RAWMOUSE>::operator<<(const RAWINPUT & ri)
	{
		ri_data_ = ri.data.mouse; return *this;
	}

	template<> Device<RAWKEYBOARD> & Device<RAWKEYBOARD>::operator<<(const RAWINPUT & ri)
	{
		ri_data_ = ri.data.keyboard; return *this;
	}

	template<> Device<RAWHID> & Device<RAWHID>::operator<<(const RAWINPUT & ri)
	{
		ri_data_ = ri.data.hid; return *this;
	}

	template<> std::wstring & Device<RAWMOUSE>::data_str()
	{
		std::wstringstream ss;

		ss << ri_data_.usFlags
			//<< L'\t' << ri_data_.ulButtons
			<< L"\t[" << ri_data_.usButtonFlags << L"]"
			<< L'\t' << short(ri_data_.usButtonData)
			<< L"\t[" << ri_data_.ulRawButtons << L']'
			<< L'\t' << ri_data_.lLastX
			<< L'\t' << ri_data_.lLastY
			//<< L'\t' << ri_data_.ulExtraInformation
			<< L'\n';

		return str_data_ = ss.str();
	}

	template<> std::wstring & Device<RAWKEYBOARD>::data_str()
	{
		std::wstringstream ss;

		ss << ri_data_.MakeCode
			<< L"\t[" << ri_data_.Flags << L']'
			<< L"\t[" << ri_data_.VKey << L']'
			<< L'\t' << ri_data_.Message
			//<< L'\t' << ri_data_.ExtraInformation
			<< L'\n';

		return str_data_ = ss.str();
	}

	template<> std::wstring & Device<RAWHID>::data_str()
	{
		std::wstringstream ss;

		int tot = ri_data_.dwSizeHid * ri_data_.dwCount;

		for (int i = 0; i < tot - 1; ++i) ss << ri_data_.bRawData[i] << L'\t';

		return str_data_ = ss.str() + L'\n';
	}
}