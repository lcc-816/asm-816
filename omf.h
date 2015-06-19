
#ifndef __omf_h__
#define __omf_h__

#include <vector>
#include <string>
#include <stdint.h>

namespace OMF {

	class Segment {
	public:
		uint32_t resspace = 0;
		uint32_t length = 0;
		uint16_t kind = 0;
		uint32_t banksize = 0;
		uint32_t org = 0;
		uint32_t alignment = 0;
		uint16_t segnum = 0;
		uint32_t entry = 0;

		std::string segname;
		std::string loadname;

		std::vector<uint8_t> body;


		void write(int fd);
	};


	class SegmentBuilder {

	public:

		std::vector<uint8_t> body;
		uint32_t length = 0;

		uint32_t pc() const;

		template <class InputIt>
		void raw_append(InputIt begin, InputIt end, uint32_t lsize = 0) {
			save_lconst();
			append(begin, end);
			length += lsize;
		}

		template <class InputIt>
		void data(InputIt begin, InputIt end) {
			lconst.insert(lconst.end(), begin, end);
		}

		void data(uint32_t x, unsigned bytes) {
			while (bytes) {
				lconst.push_back(x & 0xff);
				x >>= 8;
				bytes--;
			}
		}

		void data(uint8_t x) {
			lconst.push_back(x);
		}

		void data(uint16_t x) {
			lconst.push_back((x >> 0) & 0xff);
			lconst.push_back((x >> 8) & 0xff);
		}

		void data(uint32_t x) {
			lconst.push_back((x >> 0 ) & 0xff);
			lconst.push_back((x >> 8 ) & 0xff);
			lconst.push_back((x >> 16) & 0xff);
			lconst.push_back((x >> 24) & 0xff);
		}

		void ds(uint32_t size);
		void end();
		void entry(uint16_t segnum, const std::string &label);

		void global(const std::string &label, uint16_t length, uint8_t type, bool exported = true);
		void local(const std::string &label, uint16_t length, uint8_t type) {
			global(label, length, type, false);
		}

		void strong(const std::string &);
		void use(const std::string &);

	private:

		void append(uint8_t x) {
			body.push_back(x);
		}

		void append(uint16_t x) {
			body.push_back((x >> 0) & 0xff);
			body.push_back((x >> 8) & 0xff);
		}

		void append(uint32_t x) {
			body.push_back((x >> 0 ) & 0xff);
			body.push_back((x >> 8 ) & 0xff);
			body.push_back((x >> 16) & 0xff);
			body.push_back((x >> 24) & 0xff);
		}

		void append(const std::string &);
		void append(uint8_t opcode, const std::string &);

		void append(uint8_t opcode, uint32_t numlen) {
			append(opcode);
			append(numlen);
		}


		template <class InputIt>
		void append(InputIt begin, InputIt end, uint32_t size = 0) {
			body.insert(body.end(), begin, end);
			length += size;
		}

		std::vector<uint8_t> lconst;

		void save_lconst();

	};

	enum {
		END_OF_SEGMENT = 0x00,
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

	enum {
		KIND_CODE = 0x00,
		KIND_DATA = 0x01,
		KIND_JUMP_TABLE = 0x02,
		KIND_PATHNAME = 0x04,
		KIND_LIBRARY = 0x08,
		KIND_INIT = 0x10,
		KIND_DP = 0x12,

		ATTR_BANK_RELATIVE = 1 << 8,
		ATTR_SKIP = 1 << 9,
		ATTR_RELOAD =  1 << 10,
		ATTR_ABSOLUTE = 1 << 11,
		ATTR_SPECMEM = 1 << 12,
		ATTR_PIC = 1 << 13,
		ATTR_PRIVATE = 1 << 14,
		ATTR_DYNAMIC = 1 << 15
	};

}

#endif
