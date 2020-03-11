#pragma once

#include "generated/lpc_kbd.dev.h"

namespace lpc_kbd{

	static inline bool has_key(lpc_kbd_t& dev){
		// fetch output buffer field from status register. return 1 if it is 1 else return 0
		return (lpc_kbd_status_obf_extract(lpc_kbd_status_rd(&dev)) == (uint8_t)0x1);
	}

	static inline uint8_t get_key(lpc_kbd_t& dev)
	{
		return lpc_kbd_input_rd(&dev);
	}

}// namespace lpc_kbd

