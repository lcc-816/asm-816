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

		// todo -- special handling for relative data (*).

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

		e = e->simplify();

		if (e->is_integer(size)) {
			builder.ds(size);
			return;
		}
		throw std::runtime_error("Expression is not absolute");
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

	void dc(OMF::SegmentBuilder &builder, ExpressionPtr e, unsigned bytes, uint32_t pc, OpCode op) {
		unsigned type = OMF::EXPR; // expression.
		AddressMode mode = op.addressMode();

		uint32_t i;

		//special handling for relative instructions.
		if (op.isRelative()) {
			int32_t branch;
			if (e->evaluate_relative(pc + 1 + bytes, branch))
			{

				// check if in range...
				if (bytes == 1) {
					if (branch >= -128 && branch <= 127) {
						builder.data((uint32_t)branch, 1);
						return;
					}
					// or throw...
					throw std::runtime_error("branch out of range");
				}
				if (bytes == 2) {
					if (branch >= -32768 && branch <= 32767) {
						// mmmm will wrap at $ffff so all numbers reachable.
						builder.data((uint32_t)branch, 2);
						return;
					}
					throw std::runtime_error("branch out of range");
				}

				return;
			}

			std::vector<uint8_t> tmp = e->to_omf(OMF::RELEXPR, bytes, bytes); // 0 displacement?
			builder.raw_append(tmp.begin(), tmp.end(), bytes);

			return;
		}


		if (e->is_integer(i)) {
			builder.data(i, bytes);
			return;
		}

		const std::string *s;
		if (e->is_string(s)) {
			builder.data(s->begin(), s->end());

			// zero-pad to boundary.
			int mod = s->size() % bytes;
			builder.data(0, mod);
			return;
		}


		switch(op.mnemonic()) {
			case JSL:
				type = OMF::LEXPR; // lexpr
				break;
			case JMP:
				// jmp (abs) actually uses bank 0.
				if (mode == absolute_indirect)
					break;
			case JSR:
				type = OMF::BKEXPR; // bkexpr
				break;
			default:
				break;
		}
		std::vector<uint8_t> tmp = e->to_omf(type, bytes);
		builder.raw_append(tmp.begin(), tmp.end(), bytes);
		// 
	}

}


extern void assign_pc(LineQueue &lines);


OMF::Segment data_to_omf(Segment *segment) {

	OMF::SegmentBuilder builder;

	assign_pc(segment->lines); //


	for (auto &line : segment->lines) {

		if (line->label) {
			auto label = line->label;

			// create a global label
			builder.global(*label, 0, 0x4e, line->global);

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

	// strong references
	for (auto id : segment->strong_vector) {
		if (id) builder.strong(*id);
	}

	builder.end();
	//
	OMF::Segment seg;
	seg.length = builder.length;
	seg.body = std::move(builder.body);
	seg.kind = segment->kind;
	if (seg.kind == 0) seg.kind = OMF::KIND_DATA | OMF::ATTR_PRIVATE; // data, static, private
	if (segment->global) seg.kind &= ~OMF::ATTR_PRIVATE; // public.

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



OMF::Segment code_to_omf(Segment *segment) {

	// this operates on basic blocks.


	OMF::SegmentBuilder builder;

	assign_pc(segment->lines); // todo -- move this elsewhere. optimizer will set pc as part of fixing branches.


	for (auto line : segment->lines) {

		if (line->label) {
			// code -- only include global labels.
			if (line->global)
				builder.global(*line->label, 0, 'N', line->global);
			continue;
		}

		if (line->directive != kUndefinedDirective) {


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

				default:
					fprintf(stderr, "Invalid code directive\n");
					exit(1);
			}


			continue;
		}

		// show time!  opcode and operands.
		{
			OpCode op = line->opcode;
			builder.data(op.opcode());
			unsigned bytes;

			// MVP / MVN are special cases.
			if (op.mnemonic() == MVN || op.mnemonic() == MVP) {
				dc(builder,line->operands[1], 1);
				dc(builder,line->operands[0], 1);
				continue;
			}
			// also, 

			// todo -- relative / relative long addressing needs REL expr.
			// dp operands s/b ZPEXPR
			// abs operands s/b BKEXPR [?]
			bytes = op.bytes(line->longM, line->longX);
			if (bytes > 1) {
				dc(builder, line->operands[0], bytes - 1, line->pc, op);
			}

		}
	}

	// strong references
	for (auto id : segment->strong_vector) {
		if (id) builder.strong(*id);
	}


	builder.end();
	//
	OMF::Segment seg;
	seg.length = builder.length;
	seg.body = std::move(builder.body);
	seg.kind = segment->kind;
	if (segment->global) seg.kind &= OMF::ATTR_PRIVATE; // public.
	seg.banksize = 0x010000;

	if (segment->name) seg.segname = *segment->name;
	if (segment->segment) seg.loadname = *segment->segment;

	return seg;
}
