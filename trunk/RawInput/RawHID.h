#ifndef RawHID_h__
#define RawHID_h__

#include "rawdevice.h"

namespace RawInput
{
	class RawHID : public RawDevice {
	public:
		explicit RawHID(void);

		virtual ~RawHID(void);

		virtual void Read(const RAWINPUT &);

		virtual void Clean(void);

		const RAWHID & GetData(void) const;
	private:
		RAWHID m_data;
	};
}

#endif // RawHID_h__