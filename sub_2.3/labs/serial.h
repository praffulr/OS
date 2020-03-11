#pragma once

#include "util/io.h"

namespace serial{

	static inline bool is_transmitter_ready(io_t baseport){
		// read from 5th offset 8 bit register
		uint8_t line_status = io::read8(baseport, 5);
		// if 5th bit from lsb side is 1 then
		// return true else false
		return ((line_status & (uint8_t)0x20) == (uint8_t)0x20);
	}

	static inline void writechar(uint8_t c, io_t baseport){
		// write 8 bits into 0th offset register
		io::write8(baseport, 0, c);
	}

} //end serial
