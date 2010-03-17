#pragma once

#include "RawInputAPI.h"

namespace RawInput
{
	struct RawDevice {
		virtual ~RawDevice(void);

		virtual const RAWINPUT & Read(const RAWINPUT & ri) = 0;

		virtual void Clean(void) = 0;
	};

	class RawMouse : public RawDevice {
	public:
		virtual ~RawMouse(void);

		const RAWINPUT & Read(const RAWINPUT & ri);

		void Clean(void);

		const RAWMOUSE & GetData(void);

		bool Button(unsigned short) const;

		bool ButtonHeld(unsigned short) const;

		void GetPosXY(long *, long *) const;

		short GetWheelDelta(void) const;
	private:
		RAWMOUSE ri_data_;
	};

	class RawKeyboard : public RawDevice {
	public:
		virtual ~RawKeyboard(void);

		const RAWINPUT & Read(const RAWINPUT & ri);

		void Clean(void);

		const RAWKEYBOARD & GetData(void);

		bool KeyUp(unsigned short) const;

		bool KeyDown(unsigned short) const;

		bool KeyHeld(unsigned short) const;
	private:
		RAWKEYBOARD ri_data_;
	};

	class RawHID : public RawDevice {
	public:
		virtual ~RawHID(void);

		const RAWINPUT & Read(const RAWINPUT & ri);

		void Clean(void);

		const RAWHID & GetData(void);
	private:
		RAWHID ri_data_;
	};
}