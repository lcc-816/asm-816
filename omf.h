
#ifndef __omf_h__
#define __omf_h__

#include <vector>
#include <string>
#include <stdint.h>

namespace OMF {

	class Segment {
	public:
		uint32_t length = 0;
		uint32_t resspace = 0;
		uint16_t kind = 0;
		uint32_t banksize = 0;
		std::string segname;
		std::string loadname;

		std::vector<uint8_t> data;
	};

	enum {
		ALIGN = 0xe0,
		ORG = 0xe1,
		RELOC = 0xe2,
		INTERSEG = 0xe3,
		USING = 0xe4,
		STRONG = 0xe5,
		GLOBAL = 0xe6,
		GEQU = 0xe7,
		// MEM - obsolete
		EXPR = 0xeb,
		ZPEXPR = 0xec,
		BKEXPR = 0xed,
		RELEXPR = 0xee,
		LOCAL = 0xef,
		EQU = 0xf0,
		DS = 0xf1,
		LCONST = 0xf2,
		LEXPR = 0xf3,
		ENTRY = 0xf4,
		cRELOC = 0xf5,
		cINTERSEG = 0xf6,
		SUPER = 0xf7,
	};

}

#endif
