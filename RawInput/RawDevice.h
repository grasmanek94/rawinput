#ifndef RawDevice_h__
#define RawDevice_h__

#include "RawInputAPI.h"

namespace RawInput
{
	class RawDevice {
		RawDevice(const RawDevice &);

		const RawDevice & operator=(const RawDevice &);
	public:
		explicit RawDevice(void) {}

		virtual ~RawDevice(void) {}

		virtual void Read(const RAWINPUT &) = 0;

		virtual void Clean(void) = 0;
	};
}

#endif // RawDevice_h__