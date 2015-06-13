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

	void dc(OMF::SegmentBuilder &builder, ExpressionPtr e, unsigned bytes);
	void dc(OMF::SegmentBuilder &builder, std::vector<ExpressionPtr> &v, unsigned bytes);

	void dc(OMF::SegmentBuilder &builder, std::vector<ExpressionPtr> &v, unsigned bytes) {
		for (auto e : v)
			dc(builder, e, bytes);
	}

	void dc(OMF::SegmentBuilder &builder, ExpressionPtr e, unsigned bytes) {

		uint32_t i;
		std::vector<ExpressionPtr> v;

		if (e->is_vector(v)) {
			dc(builder, v, bytes);
			return;
		}

		if (e->is_integer(i)) {
			builder.data(i, bytes);
			return;
		}

		std::vector<uint8_t> tmp = e->to_omf(OMF::EXPR, bytes);
		builder.raw_append(tmp.begin(), tmp.end(), bytes);
	}

	void ds(OMF::SegmentBuilder &builder, ExpressionPtr e) {
		// DS #
		// DS $1000-* ? 
		uint32_t size;

		if (e->is_integer(size)) {
			builder.ds(size);
			return;
		}
		fprintf(stderr, "ds - expression too complicated\n");
		exit(1);

	}

	void align(OMF::SegmentBuilder &builder, ExpressionPtr e) {

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

	OMF::SegmentBuilder builder;

	for (auto &line : segment->lines) {

		if (line->label) {
			auto label = line->label;

			// create a global label
			builder.global(*label, 0, 0x4e, export_set.find(label) != export_set.end());

			continue;
		}
		switch (line->directive) {
			case DCB:
				dc(builder, line->operands[0], 1);
				break;
			case DCW:
				dc(builder, line->operands[0], 2);
				break;
			case DCL:
				dc(builder, line->operands[0], 4);
				break;
			case DS:
				ds(builder, line->operands[0]);
				break;
			case ALIGN:
				align(builder, line->operands[0]);
				break;

			default:
				fprintf(stderr, "Invalid data directive\n");
				exit(1);

		}

	}

	builder.end();
	//
	OMF::Segment seg;
	seg.length = builder.length;
	seg.body = std::move(builder.body);
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
