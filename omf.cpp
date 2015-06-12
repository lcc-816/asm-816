
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
		bytecount = headersize + data.size(); 

		header.reserve(headersize);

		push_back(header, bytecount, 4);
		push_back(header, resspace, 4);
		push_back(header, data.size(), 4);
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
			rv = ::write(fd, data.data(), data.size());
			if (rv < 0) {
				if (errno == EINTR) continue;
				throw std::system_error(errno, std::system_category(), "write");
			}
			break;
		}

	}

}
