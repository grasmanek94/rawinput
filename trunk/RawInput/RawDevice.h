#pragma once

#include "RawInputAPI.h"

namespace RawInput
{
 	struct RAWINPUT_API Device_base {
		virtual ~Device_base() {}
		virtual Device_base & operator<<(const RAWINPUT & ri) = 0;
	};

	template<typename T> class RAWINPUT_API Device : public Device_base {};

	template<> class Device<RAWMOUSE> : public Device_base {
	public:
		Device_base & operator<<(const RAWINPUT & ri) { ri_data_ = ri.data.mouse; return *this; }

		void SetData(const RAWMOUSE & data) { ri_data_ = data; }
		const RAWMOUSE & GetData() { return ri_data_; }
		void Clean() { ri_data_ = RAWMOUSE(); }
	private:
		RAWMOUSE ri_data_;
	};

	template<> class Device<RAWKEYBOARD> : public Device_base {
	public:
		Device_base & operator<<(const RAWINPUT & ri) { ri_data_ = ri.data.keyboard; return *this; }

		void SetData(const RAWKEYBOARD & data) { ri_data_ = data; }
		const RAWKEYBOARD & GetData() { return ri_data_; }
		void Clean() { ri_data_ = RAWKEYBOARD(); }
	private:
		RAWKEYBOARD ri_data_;
	};

	template<> class Device<RAWHID> : public Device_base {
	public:
		Device_base & operator<<(const RAWINPUT & ri) { ri_data_ = ri.data.hid; return *this; }

		void SetData(const RAWHID & data) { ri_data_ = data; }
		const RAWHID & GetData() { return ri_data_; }
		void Clean() { ri_data_ = RAWHID(); }
	private:
		RAWHID ri_data_;
	};

	template <typename T> T & operator<<(T & out, Device<RAWMOUSE> dev)
	{
		out << dev.GetData().usFlags
			//<< L'\t' << dev.GetData().ulButtons
			<< L"\t[" << dev.GetData().usButtonFlags << L"]"
			<< L'\t' << short(dev.GetData().usButtonData)
			<< L"\t[" << dev.GetData().ulRawButtons << L']'
			<< L'\t' << dev.GetData().lLastX
			<< L'\t' << dev.GetData().lLastY
			//<< L'\t' << dev.GetData().ulExtraInformation
			<< L'\n';

		return out;
	}

	template <typename T> T & operator<<(T & out, Device<RAWKEYBOARD> dev)
	{
		out << dev.GetData().MakeCode
			<< '\t' << dev.GetData().Flags
			<< "\t[" << dev.GetData().VKey << ']'
			<< '\t' << dev.GetData().Message
			//<< '\t' << dev.GetData().ExtraInformation
			<< '\n';

		return out;
	}

	template <typename T> T & operator<<(T & out, Device<RAWHID> dev)
	{
		int tot = dev.GetData().dwSizeHid * dev.GetData().dwCount;

		for (int i = 0; i < tot - 1; ++i) out << dev.GetData().bRawData[i] << L'\t';

		return out << L'\n';
	}
}