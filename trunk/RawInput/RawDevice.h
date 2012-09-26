#ifndef RawDevice_h__
#define RawDevice_h__

#include "RawInputAPI.h"

#include <functional>
#include <memory>

namespace RawInput
{
	class RawDevice {
		RawDevice(const RawDevice &);

		const RawDevice & operator=(const RawDevice &);
	public:
		typedef HANDLE Handle;

		typedef std::shared_ptr<RawDevice> Ptr;

		//http://www.microsoft.com/whdc/archive/HID_HWID.mspx
		enum usage_page {
			HID_USAGE_PAGE				= 0x01
		};

		enum usage {
			HID_DEVICE_SYSTEM_MOUSE		= 0x02,
			HID_DEVICE_SYSTEM_KEYBOARD	= 0x06,
			HID_DEVICE_SYSTEM_GAME		= 0x04
		};

		explicit RawDevice(void) {}

		virtual ~RawDevice(void) {}

		virtual void Read(const RAWINPUT &) = 0;

		template <class DevType>
		struct DeviceEvent : public std::function<void(const DevType &)> {

			DeviceEvent(const typename DeviceEvent::function && callable)
				: function<void(const DevType &)>(callable)
			{
			}
		};
	};
}

#endif // RawDevice_h__
