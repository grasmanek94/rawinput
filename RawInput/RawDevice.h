#ifndef RawDevice_h__
#define RawDevice_h__

#include "RawInputAPI.h"

#include <memory>

namespace RawInput
{
	class RawDevice {
		RawDevice(const RawDevice &);

		const RawDevice & operator=(const RawDevice &);
	public:
		explicit RawDevice(void) {}

		virtual ~RawDevice(void) {}

		virtual void Read(const RAWINPUT &) = 0;

		template <class DevType>
		struct BaseEvent {
			typedef DevType DeviceType;

			typedef std::shared_ptr<BaseEvent> Ptr;

			virtual void operator()(const DeviceType & device) = 0;
		};
	};
}

#endif // RawDevice_h__
