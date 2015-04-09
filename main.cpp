
#include <string>

#include <cstdio>
#include <unistd.h>

#include "common.h"
#include "Instruction.h"
#include "Machine.h"
#include "Expression.h"



bool parse_file(const std::string &filename, std::deque<BasicLine *> &lines);

void simplify(LineQueue &lines) {

	for (BasicLine *line : lines) {
		if (line->operands[0])
			line->operands[0] = line->operands[0]->simplify();

		if (line->operands[1])
			line->operands[1] = line->operands[1]->simplify();

	}
}

void print(const LineQueue &lines) {

	for (const BasicLine *line : lines) {

		if (line->label) { printf("%s:\n", line->label->c_str()); }

		if (line->directive) {
			std::string s;
			const char *name = "";
			switch(line->directive) {
				case DCB: name="DC.B"; break;
				case DCW: name="DC.W"; break;
				case DCL: name="DC.L"; break;
				case DS: name="DS"; break;
				case START: name="START"; break;
				case DATA: name="DATA"; break;
				case END: name="END"; break;
				case EXPORT: name="EXPORT"; break;
				case PRAGMA: name="PRAGMA"; break;
			}

			if (line->operands[0]) {
				s = line->operands[0]->to_string();
			}
			printf("    %s %s\n", name, s.c_str());
			continue;
		}

		if (line->opcode) {
			std::string op;
			AddressMode mode = line->opcode.addressMode();

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
					op = "("; // (< ?
					break;

				case zp:
				case zp_x:
				case zp_y:
				case zp_relative:
					op = "<";
					break;

				case absolute:
				case absolute_x:
				case absolute_y:
					op = "|";
					break;

				case absolute_long:
				case absolute_long_x:
					op = ">";
					break;

				case immediate:
					op = "#";
					break;

				case zp_indirect:
				case zp_indirect_x:
				case zp_indirect_y:
					op = "(<";
					break;

				case zp_indirect_long:
				case zp_indirect_long_y:
					op = "[<";
					break;

				case absolute_indirect:
				case absolute_indirect_x:
					op = "(|";
					break;

				case absolute_indirect_long:
					op = "[|";  // ??
					break;

			}

			if (mode != implied) {
				op.append(line->operands[0]->to_string());
			}
			if (mode == block || mode == zp_relative) {
				op.push_back(',');
				op.append(line->operands[1]->to_string());
			}

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
					op.append(",x");
					break;
				case zp_y:
				case absolute_y:
					op.append(",y");
					break;
				case zp_indirect:
				case absolute_indirect:
					op.append(")");
					break;
				case zp_indirect_y:
					op.append("),y");
					break;
				case zp_indirect_x:
				case absolute_indirect_x:
					op.append(",x)");
					break;
				case stack_relative:
					op.append(",s");
					break;
				case stack_relative_y:
					op.append(",s),y");
					break;

				case absolute_indirect_long:
				case zp_indirect_long:
					op.append("]");
					break;

				case zp_indirect_long_y:
					op.append("],y");
					break;
			}


			printf("    %s %s\n", 
				line->opcode.toString(), 
				op.c_str()
			);
		}
	}
}

int main(int argc, char **argv) {

	int c;
	while ((c = getopt(argc, argv, "")) != -1) {


	}
	argv += optind;
	argc -= optind;


	for (int i = 0 ; i < argc; ++i) {

		LineQueue lines;
		bool ok = parse_file(argv[i], lines);
		if (ok) {
			simplify(lines);
			print(lines);
			printf("-------\n");
			//line = peephole(lines);
			print(lines);
			printf("-------\n");
		}

		for (BasicLine *tmp : lines) { delete tmp; }
		lines.clear();
	}

	return 0;
}

