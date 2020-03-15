#include "util/io.h"

namespace vgatext{

   	static inline void writechar(int loc, uint8_t c, uint8_t bg, uint8_t fg, addr_t base)
	{
		uint16_t val = ( (c)) + ( (bg << 12) ) + ( (fg << 8) );
		mmio::write16(base, (uint32_t)(loc*2), val);
	}

}//namespace vgatext
