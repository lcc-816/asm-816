#include <string>

#include "printer.h"
#include "Expression.h"
#include "omf.h"
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
	const char *type;

	int attr = segment->kind & 0xff00;
	int kind = segment->kind & 0x00ff;
	switch (kind) {
		case OMF::KIND_DATA:
			type = "RECORD";
			break;
		case OMF::KIND_CODE:
		default:
			type = "PROC";
			break;
		case OMF::KIND_INIT:
			type = "INIT";
			break;
		case OMF::KIND_DP:
			type = "STACKDP";
			break;
	}

	// todo -- align, org, skip included w/ header line
	std::string sa;
	if (attr & OMF::ATTR_RELOAD) {
		sa += "RELOAD";
	} else {
		sa += "NORELOAD";
	}
	// todo BANK xx / NOBANK
	if (attr & OMF::ATTR_SPECMEM) {
		sa += ",SPECIAL";
	} else {
		sa += ",NOSPECIAL";
	}

	if (attr & OMF::ATTR_DYNAMIC) {
		sa += ",DYNAMIC";
	} else {
		sa += ",STATIC";
	}


	fprintf(f, "    SEGATTR %s\n", sa.c_str());

	fprintf(f, "%s    %s\n", name ? name->c_str() : "", type);

}

void mpw_printer::end_segment(FILE *f, const SegmentPtr &segment) {
	int kind = segment->kind & 0x00ff;
	const char *type;
	switch (kind) {
		case OMF::KIND_DATA:
			type = "ENDR";
			break;
		case OMF::KIND_CODE:
		default:
			type = "ENDP";
			break;
		case OMF::KIND_INIT:
			type = "ENDI";
			break;
		case OMF::KIND_DP:
			type = "ENDS";
			break;
	}
	fprintf(f, "    %s\n\n", type);
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


#pragma mark - merlin

void merlin_printer::begin(FILE *f) {
	fprintf(f, "    xc\n");
	fprintf(f, "    xc\n");
	fprintf(f, "\n");
}

void merlin_printer::end(FILE *f) {
}

void merlin_printer::begin_segment(FILE *f, const SegmentPtr &segment) {
	identifier name = segment->name;
	fprintf(f, "%s\n", name ? name->c_str() : "");
}

void merlin_printer::end_segment(FILE *f, const SegmentPtr &segment) {
}


void merlin_printer::print_data(FILE *f, const BasicLinePtr &line) {

	if (!line->directive) return;
	if (!line->operands[0]) return;

	std::string s = line->operands[0]->to_string();

	switch(line->directive) {

		case DCB:
			fprintf(f, "    db %s\n", s.c_str());
			break;
		case DCW:
			fprintf(f, "    dw %s\n", s.c_str());
			break;
		case DCL:
			fprintf(f, "    adrl %s\n", s.c_str());
			break;
		case DS:
			fprintf(f, "    ds %s\n", s.c_str());
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
	const char *type;

	int attr = segment->kind & 0xff00;
	int kind = segment->kind & 0x00ff;
	switch (kind) {
		case OMF::KIND_DATA:
			type = attr & OMF::ATTR_PRIVATE ? "PRIVDATA" : "DATA";
			break;
		case OMF::KIND_CODE:
		default:
			type = attr & OMF::ATTR_PRIVATE ? "PRIVATE" : "START";
			break;
	}

	fprintf(f, "%s    %s\n", name ? name->c_str() : "", type);

	switch(segment->kind) {
		case OMF::KIND_DATA:
		case OMF::KIND_CODE:
		case OMF::KIND_DATA | OMF::ATTR_PRIVATE:
		case OMF::KIND_CODE | OMF::ATTR_PRIVATE:
			break;
		default:
			fprintf(f, "    KIND $%04x\n", segment->kind);
			break;
	}
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
			fprintf(f, "    dc i1'%s'\n", s.c_str());
			break;
		case DCW:
			fprintf(f, "    dc i2'%s'\n", s.c_str());
			break;
		case DCL:
			fprintf(f, "    dc i4'%s'\n", s.c_str());
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
	const char *type = "function";
	int kind = segment->kind;
	int attr = kind & 0xff00;

	switch(kind & 0xff) {
		case OMF::KIND_DATA:
			type = "data";
			break;
		case OMF::KIND_CODE:
			type = "function";
			break;
		default:
			type = "function";
			break;
	}


	fprintf(f, "%s %s {\n", type, name ? name->c_str() : "");

	switch(kind & 0xff) {
		case OMF::KIND_CODE:
		case OMF::KIND_DATA:
			switch (attr) {
				case 0:
					fprintf(f, "    pragma public\n");
					break;
				case OMF::ATTR_PRIVATE:
					fprintf(f, "    pragma private\n");
					break;
				case OMF::ATTR_DYNAMIC:
					fprintf(f, "    pragma dynamic,public\n");
					break;

				case OMF::ATTR_PRIVATE | OMF::ATTR_DYNAMIC:
					fprintf(f, "    pragma dynamic,private\n");
					break;
				default:
					fprintf(f, "    pragma kind=$%04x\n", kind);
					break;
			}
			break;
		default:
			fprintf(f, "    pragma kind=$%04x\n", kind);
			break;
	}


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
