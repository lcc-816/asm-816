
#include "omf.h"
#include <vector>
#include <unistd.h>
#include <system_error>
#include <cerrno>
#include <cassert>

namespace {
	void push_back(std::vector<uint8_t> &v, uint32_t t, unsigned bytes) {
		for (unsigned i = 0; i < bytes; ++i) {
			v.push_back(t & 0xff);
			t >>= 8;
		}
	}
}

namespace OMF {

	void Segment::write(int fd) {

		const uint32_t dispname = 0x30;

		std::vector<uint8_t> header;
		uint32_t bytecount;
		uint32_t headersize;

		//bytecount includes header.
		headersize = 0x30 + 10 + segname.size() + 1;
		bytecount = headersize + body.size(); 

		header.reserve(headersize);

		push_back(header, bytecount, 4);
		push_back(header, resspace, 4);
		push_back(header, length, 4);
		push_back(header, 0, 1); // unused
		push_back(header, 0, 1); // lablen
		push_back(header, 4, 1); // numlen
		push_back(header, 2, 1); // headerersion
		push_back(header, banksize, 4);
		push_back(header, kind, 2);
		push_back(header, 0, 2); // unused
		push_back(header, 0, 4); // org
		push_back(header, alignment, 4);
		push_back(header, 0, 1); // numsex
		push_back(header, 0, 1); // unused
		push_back(header, segnum, 2);
		push_back(header, entry, 4); 
		push_back(header, dispname, 2); // dispname  
		push_back(header, headersize, 2); // dispdata  
		push_back(header, 0, 4); // temp org -- unused.

		if (loadname.size() < 10) {
			header.insert(header.end(), loadname.begin(), loadname.end());
			for (unsigned i = loadname.size(); i < 10; ++i) header.push_back(' ');
		} else {
			header.insert(header.end(), loadname.begin(), loadname.begin() + 10);
		}

		header.push_back(segname.size());
		header.insert(header.end(), segname.begin(), segname.end());

		assert(header.size() == headersize);


		for(;;) {
			ssize_t rv;
			rv = ::write(fd, header.data(), header.size());
			if (rv < 0) {
				if (errno == EINTR) continue;
				throw std::system_error(errno, std::system_category(), "write");
			}
			break;
		}

		for(;;) {
			ssize_t rv;
			rv = ::write(fd, body.data(), body.size());
			if (rv < 0) {
				if (errno == EINTR) continue;
				throw std::system_error(errno, std::system_category(), "write");
			}
			break;
		}

	}


	void SegmentBuilder::save_lconst() {
		size_t size = lconst.size();
		if (size == 0) return;

		if (size <= 0xdf) append((uint8_t)size);
		else append((uint32_t)size);

		append(lconst.begin(), lconst.end());
		lconst.clear();
		length += size;
	}

	void SegmentBuilder::append(const std::string &s){
		size_t size = s.size();
		if (size == 0 || size > 0xff) {
			throw std::invalid_argument("invalid string size");
		}

		append((uint8_t)size);
		append(s.begin(), s.end());
	}

	void SegmentBuilder::append(uint8_t opcode, const std::string &s){
			append((uint8_t)opcode);
			append(s);
	}

#if 0
	void SegmentBuilder::align(uint32_t size) {
		save_lconst();
		append(OMF::ALIGN, size);
		// todo -- size must be power of 2.
		//length += size; ???
	}

	void SegmentBuilder::org(uint32_t size) {
		save_lconst();
		append(OMF::ORG, size);
		//length += size; ???
	}
#endif

	void SegmentBuilder::ds(uint32_t size) {
		save_lconst();

		append(OMF::DS, size);
		length += size;
	}

	void SegmentBuilder::end() {
		save_lconst();
		append((uint8_t)OMF::END_OF_SEGMENT);
	}

	void SegmentBuilder::entry(uint16_t segnum, const std::string &label) {
		save_lconst();

		append((uint8_t)OMF::ENTRY);
		append((uint16_t)segnum);
		append(label);
	}


	void SegmentBuilder::global(const std::string &label, uint16_t length, uint8_t type, bool exported) {
		save_lconst();

		append((uint8_t)OMF::GLOBAL, label);
		append((uint16_t)length);
		append((uint8_t)type);
		append(exported ? (uint8_t)0 : (uint8_t)1);
	}

	void SegmentBuilder::strong(const std::string &name) {
		save_lconst();

		append(OMF::STRONG, name);
	}

	void SegmentBuilder::use(const std::string &name) {
		save_lconst();
		
		append(OMF::USING, name);
	}



}
