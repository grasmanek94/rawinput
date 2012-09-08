#ifndef RawMouse_h__
#define RawMouse_h__

#include "rawdevice.h"

namespace RawInput
{
	class RawMouse : public RawDevice {
	public:
		enum mouse_flags {
			BUTTON_1_DOWN	= RI_MOUSE_BUTTON_1_DOWN,
			BUTTON_1_UP		= RI_MOUSE_BUTTON_1_UP,

			BUTTON_2_DOWN	= RI_MOUSE_BUTTON_2_DOWN,
			BUTTON_2_UP		= RI_MOUSE_BUTTON_2_UP,

			BUTTON_3_DOWN	= RI_MOUSE_BUTTON_3_DOWN,
			BUTTON_3_UP		= RI_MOUSE_BUTTON_3_UP,

			BUTTON_4_DOWN	= RI_MOUSE_BUTTON_4_DOWN,
			BUTTON_4_UP		= RI_MOUSE_BUTTON_4_UP,

			BUTTON_5_DOWN	= RI_MOUSE_BUTTON_5_DOWN,
			BUTTON_5_UP		= RI_MOUSE_BUTTON_5_UP,

			WHEEL			= RI_MOUSE_WHEEL
		};

		explicit RawMouse(void);

		virtual ~RawMouse(void);

		virtual void Read(const RAWINPUT &);

		virtual void Clean(void);

		const RAWMOUSE & GetData(void) const;

		/*
		 * Button() returns true on button event.
		 *
		 * @param1: one of Mouse::mouse_flags enum.
		 */
		bool Button(unsigned short) const;

		/*
		 * GetPosXY() returns mouse's x/y position.
		 *
		 * @param1: returns mouse last position X.
		 * @param2: returns mouse last position Y.
		 */
		void GetPosXY(long *, long *) const;

		/*
		 * GetWheelDelta() returns mouse's wheel movement.
		 */
		short GetWheelDelta(void) const;
	private:
		RAWMOUSE m_data;
	};
}

#endif // RawMouse_h__
