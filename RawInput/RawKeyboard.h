#ifndef RawKeyboard_h__
#define RawKeyboard_h__

#include "rawdevice.h"

namespace RawInput
{
	class RawKeyboard : public RawDevice {
	public:
		explicit RawKeyboard(void);

		virtual ~RawKeyboard(void);

		virtual void Read(const RAWINPUT &);

		virtual void Clean(void);

		const RAWKEYBOARD & GetData(void) const;

		/*
		 * KeyUp() returns true on key release.
		 *
		 * @param1: one VK code.
		 */
		bool KeyUp(unsigned short) const;

		/*
		 * KeyDown() returns true on key press.
		 *
		 * @param1: one VK code.
		 */
		bool KeyDown(unsigned short) const;

		/*
		 * KeyHeld() returns true while key is pressed.
		 *
		 * @param1: one VK code.
		 */
		bool KeyHeld(unsigned short) const;
	private:
		RAWKEYBOARD m_data;
	};
}

#endif // RawKeyboard_h__