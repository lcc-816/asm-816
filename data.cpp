/*
 * data segment to omf.
 */

#include <stdlib.h>

#include "common.h"
#include "omf.h"
#include "Expression.h"


/*
 * this assumes little endian NUMSEX, 32-bit NUMLEN, variable length LABLEN.
 *
 */
namespace {

	size_t data_size = 0;
	std::vector<uint8_t> data;
	std::vector<uint8_t> lconst;



	void save_lconst() {
		size_t size = lconst.size();

		if (size == 0) return;

		if (size <= 0xdf) {
			data.push_back((uint8_t)size);
		} else {
			data.push_back(OMF::LCONST); // LCONST
			data.push_back((size >> 0 ) & 0xff);
			data.push_back((size >> 8 ) & 0xff);
			data.push_back((size >> 16) & 0xff);
			data.push_back((size >> 24) & 0xff);
		}

		data.insert(data.end(), lconst.begin(), lconst.end());

		lconst.clear();
		data_size += size;
	}

	void global_label(const std::string *label, uint16_t length, uint8_t type, bool exported) {
		save_lconst();

		data.push_back(OMF::GLOBAL);
		data.push_back(label->length());
		data.insert(data.end(), label->begin(), label->end());
		data.push_back((length >> 0) & 0xff); 
		data.push_back((length >> 8) & 0xff); 
		data.push_back(type); // type
		data.push_back(exported ? 0 : 1);
	}


	void dc(ExpressionPtr e, unsigned bytes);
	void dc(std::vector<ExpressionPtr> &v, unsigned bytes);

	void dc(std::vector<ExpressionPtr> &v, unsigned bytes) {
		for (auto e : v)
			dc(e, bytes);
	}

	void dc(ExpressionPtr e, unsigned bytes) {

		uint32_t i;
		std::vector<ExpressionPtr> v;

		if (e->is_vector(v)) {
			dc(v, bytes);
			return;
		}

		if (e->is_integer(i)) {
			while (bytes) {
				lconst.push_back(i & 0xff);
				--bytes;
				i >>= 8;
			}
			return;
		}
		save_lconst();

		std::vector<uint8_t> tmp = e->to_omf(OMF::EXPR, bytes);
		data.insert(data.end(), tmp.begin(), tmp.end());
		data_size += bytes;
	}

	void ds(ExpressionPtr e) {
		// DS #
		// DS $1000-* ? 
		uint32_t size;
		save_lconst();

		if (e->is_integer(size)) {
			data.push_back(OMF::DS);
			data.push_back((size >> 0 ) & 0xff);
			data.push_back((size >> 8 ) & 0xff);
			data.push_back((size >> 16) & 0xff);
			data.push_back((size >> 24) & 0xff);

			data_size += size;
			return;
		}
		fprintf(stderr, "ds - expression too complicated\n");
		exit(1);

	}

	void align(ExpressionPtr e) {
		save_lconst();

		// orca/m align is a ds, but requires the 
		// segment be aligned, first.
		// too complicated and not useful.
		// segments only support page and bank alignment.
		// bank alignment is not supported by the memory manager,
		// so it can cause purging as the loader emulates it.
		fprintf(stderr, "align - not supported\n");
		exit(1);
	}

}




OMF::Segment data_to_omf(Segment *segment, const std::unordered_set<const std::string *> &export_set) {


	data_size = 0;
	data.clear();
	lconst.clear();

	for (auto &line : segment->lines) {

		if (line->label) {
			auto label = line->label;

			// create a global label
			global_label(label, 0, 0x4e, export_set.find(label) != export_set.end());

			continue;
		}
		switch (line->directive) {
			case DCB:
				dc(line->operands[0], 1);
				break;
			case DCW:
				dc(line->operands[0], 2);
				break;
			case DCL:
				dc(line->operands[0], 4);
				break;
			case DS:
				ds(line->operands[0]);
				break;
			case ALIGN:
				align(line->operands[0]);
				break;

			default:
				fprintf(stderr, "Invalid data directive\n");
				exit(1);

		}

	}
	save_lconst();
	data.push_back(OMF::END_OF_SEGMENT);

	//
	OMF::Segment seg;
	seg.length = data_size;
	seg.data = std::move(data);
	seg.kind = segment->kind;
	if (seg.kind == 0) seg.kind = 0x4000; // code, static, private

	// orca has a banksize of $10000 for ~GLOBALS (and code segments), 0 for ~ARRAYS
	// orca/c uses code, static, private. ($4000)
	// apw c uses data, static, private, reload. ($4401)
	// could have separate segments for bss / data?
	// bss is 0-initialized.

	seg.banksize = 0x010000;

	if (segment->name) seg.segname = *segment->name;
	if (segment->segment) seg.loadname = *segment->segment;

	if (seg.segname.empty()) seg.segname = "~globals"; 

	return seg;
}
