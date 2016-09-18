#include <string>

#include "printer.h"
#include "Expression.h"

namespace {

	const std::string prefix(AddressMode mode) {
		switch(mode) {
			case kUndefinedAddressMode:
			case implied:
			case relative:
			case relative_long:
			case block:
			case interrupt:
			case stack_relative: // < ?
				break;

			case stack_relative_y:
				return "("; // (< ?
				break;

			case zp:
			case zp_x:
			case zp_y:
			case zp_relative:
				return "<";
				break;

			case absolute:
			case absolute_x:
			case absolute_y:
				return "|";
				break;

			case absolute_long:
			case absolute_long_x:
				return ">";
				break;

			case immediate:
				return "#";
				break;

			case zp_indirect:
			case zp_indirect_x:
			case zp_indirect_y:
			case zp_indirect_z:
				return "(<";
				break;

			case zp_indirect_long:
			case zp_indirect_long_y:
				return "[<";
				break;

			case absolute_indirect:
			case absolute_indirect_x:
				return "(|";
				break;

			case absolute_indirect_long:
				return "[|";  // ??
				break;

			default:
				return "";
		}
		return "";
	}

	std::string suffix(AddressMode mode) {

		switch(mode)
		{
			case kUndefinedAddressMode:
			case implied:
			case relative:
			case relative_long:
			case block:
			case interrupt:
			case immediate:
			case zp:
			case absolute:
			case absolute_long:
			case zp_relative:
				break;

			case zp_x:
			case absolute_x:
			case absolute_long_x:
				return ",x";
				break;
			case zp_y:
			case absolute_y:
				return ",y";
				break;
			case zp_indirect:
			case absolute_indirect:
				return ")";
				break;
			case zp_indirect_y:
				return "),y";
				break;
			case zp_indirect_z:
				return "),z";
				break;
			case zp_indirect_x:
			case absolute_indirect_x:
				return ",x)";
				break;
			case stack_relative:
				return ",s";
				break;
			case stack_relative_y:
				return ",s),y";
				break;

			case absolute_indirect_long:
			case zp_indirect_long:
				return "]";
				break;

			case zp_indirect_long_y:
				return "],y";
				break;

			default:
				return "";
		}
		return "";
	}
}
printer::~printer() {
}

void printer::print(FILE *f, const Module &m) {

	begin(f);
	for (const auto &segment : m.segments) {
		begin_segment(f, segment);

		for (const auto &line : segment->lines) {
			if (line->label) print_label(f, line);
			if (line->opcode) print_opcode(f, line);
			if (line->directive) print_data(f, line);
		}

		end_segment(f, segment);
	}

	end(f);
}


void printer::begin(FILE *f) {
}

void printer::end(FILE *f) {
}

void printer::begin_segment(FILE *f, const SegmentPtr &segment) {
}

void printer::end_segment(FILE *f, const SegmentPtr &segment) {
}


void printer::print_opcode(FILE *f, const BasicLinePtr &line) {

	if (!line->opcode) return;

	std::string op;
	AddressMode mode = line->opcode.addressMode();

	op = prefix(mode);

	if (mode != implied) {
		op.append(line->operands[0]->to_string());
	}
	if (mode == block || mode == zp_relative) {
		op.push_back(',');
		op.append(line->operands[1]->to_string());
	}

	op.append(suffix(mode));

	fprintf(f, "    %s %s\n", 
		line->opcode.toString(), 
		op.c_str()
	);
}

void printer::print_label(FILE *f, const BasicLinePtr &line) {
	identifier name = line->label;
	if (name) fprintf(f, "%s\n", name->c_str());
}


#pragma mark - mpw

void mpw_printer::begin(FILE *f) {
	fprintf(f, "    CASE ON\n\n");
}

void mpw_printer::end(FILE *f) {
	fprintf(f, "    END\n");
}

void mpw_printer::begin_segment(FILE *f, const SegmentPtr &segment) {
	identifier name = segment->name;
	fprintf(f, "%s    PROC\n", name ? name->c_str() : "");
}

void mpw_printer::end_segment(FILE *f, const SegmentPtr &segment) {
	fprintf(f, "    ENDP\n\n");
}


void mpw_printer::print_data(FILE *f, const BasicLinePtr &line) {

	if (!line->directive) return;
	if (!line->operands[0]) return;

	std::string s = line->operands[0]->to_string();

	switch(line->directive) {

		case DCB:
			fprintf(f, "    dc.b %s\n", s.c_str());
			break;
		case DCW:
			fprintf(f, "    dc.w %s\n", s.c_str());
			break;
		case DCL:
			fprintf(f, "    dc.l %s\n", s.c_str());
			break;
		case DS:
			fprintf(f, "    ds.b %s\n", s.c_str());
			break;
		default:
			return;
	}
}


#pragma mark - orca

void orca_printer::begin(FILE *f) {
	fprintf(f, "    CASE ON\n\n");
}

void orca_printer::print_label(FILE *f, const BasicLinePtr &line) {
	identifier name = line->label;
	if (name) fprintf(f, "%s    anop\n", name->c_str());
}

void orca_printer::begin_segment(FILE *f, const SegmentPtr &segment) {
	identifier name = segment->name;
	fprintf(f, "%s    START\n", name ? name->c_str() : "");
}

void orca_printer::end_segment(FILE *f, const SegmentPtr &segment) {
	fprintf(f, "    END\n\n");
}


void orca_printer::print_data(FILE *f, const BasicLinePtr &line) {

	if (!line->directive) return;
	if (!line->operands[0]) return;

	std::string s = line->operands[0]->to_string();

	switch(line->directive) {

		case DCB:
			fprintf(f, "    dc.i1 '%s'\n", s.c_str());
			break;
		case DCW:
			fprintf(f, "    dc.i2 '%s'\n", s.c_str());
			break;
		case DCL:
			fprintf(f, "    dc.i4 '%s'\n", s.c_str());
			break;
		case DS:
			fprintf(f, "    ds %s\n", s.c_str());
			break;
		default:
			return;
	}
}


#pragma mark - orca

void harpoon_printer::begin(FILE *f) {
}

void harpoon_printer::begin_segment(FILE *f, const SegmentPtr &segment) {
	identifier name = segment->name;
	fprintf(f, "funtions %s {\n", name ? name->c_str() : "");
}

void harpoon_printer::end_segment(FILE *f, const SegmentPtr &segment) {
	fprintf(f, "}\n\n");
}


void harpoon_printer::print_data(FILE *f, const BasicLinePtr &line) {

	if (!line->directive) return;
	if (!line->operands[0]) return;

	std::string s = line->operands[0]->to_string();

	switch(line->directive) {

		case DCB:
			fprintf(f, "    dc.b %s\n", s.c_str());
			break;
		case DCW:
			fprintf(f, "    dc.w %s\n", s.c_str());
			break;
		case DCL:
			fprintf(f, "    dc.l %s\n", s.c_str());
			break;
		case DS:
			fprintf(f, "    ds %s\n", s.c_str());
			break;
		default:
			return;
	}
}
