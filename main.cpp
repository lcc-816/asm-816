
#include <string>

#include <cstdio>
#include <unistd.h>

#include "common.h"
#include "Instruction.h"
#include "Machine.h"
#include "Expression.h"


Line *parse_file(const std::string &filename);

void simplify(Line *line) {
	while(line) {
		if (line->operands[0])
			line->operands[0] = line->operands[0]->simplify();

		if (line->operands[1])
			line->operands[1] = line->operands[1]->simplify();


		line = line->next;
	}
}

Line *peephole(Line *line);
#if 0
void peephole(Line *line) {

	// remove
	// BRA/BRL label
	// label:

	Line *prev = nullptr;
	while(line) {
		Line *next = line->next;

		const std::string *nextLabel = next ? next->label : nullptr;
		Instruction nextInstr; if (next) nextInstr = next->instruction;

		Instruction instr = line->instruction;

		if (instr) {

			// todo -- create DSL for these.

			// these could be generated from __builtin functions.

			/*
			 *   pea xxx
			 *   pla
			 *   ---
			 *   lda #xxx
			 */
			if (instr.mnemonic() == PEA && nextInstr.mnemonic() == PLA) {
				line->instruction = Instruction(instr.machine(), LDA);
				line->mode = immediate;
				line->explicit_mode = true;
				line->next = next->next;
				delete next;
				continue;
			}

			/*
			 *  pha
			 *  pla
			 *  ---
			 *  (nop)
			 */

			 if (instr.mnemonic() == PHA && nextInstr.mnemonic() == PLA) {
			 	Line *nn = next->next;

			 	prev->next = nn;
			 	delete line;
			 	delete next;
			 	line = nn;
			 	continue;
			 }


			/*
			 *  pei xxx
			 *  pla
			 *  ---
			 *  lda <xxx
			 */

			if (instr.mnemonic() == PEI && nextInstr.mnemonic() == PLA) {
				line->instruction = Instruction(instr.machine(), LDA);
				line->mode = zp;
				line->explicit_mode = true;

				line->next = next->next;
				delete next;
				continue;
			}

			/*
			 *  lda #const
			 *  xba
			 *  ---
			 *  lda #(swapped const)
			 *  // nb -- affects cc bits?
			 */

			if (instr.mnemonic() == LDA && line->mode == immediate && nextInstr.mnemonic() == XBA) {
				// only if expression is integer...
				uint32_t i;
				if (line->operands[0]->is_integer(i)) {

					line->operands[0] = Expression::Integer((i >> 8) | (i << 8));
					line->next = next->next;
					delete next;
					continue;
				}
			}


			/*
			 *          (BRA|BRL) label
			 * label:
			 */
			if (instr.mnemonic() == BRA || instr.mnemonic() == BRL) {

				const std::string *target;
				if (line->operands[0]->is_variable(target)) {
					if (target == nextLabel) {
						// remove current instruction.
						if (prev) {
							prev->next = next;
							delete line;
							line = next;
							continue;
						}
					}

				}
			}

			/*
			 *  STA <register
			 *  LDA <register << can be removed
			 *
			 */
			if (instr.mnemonic() == STA && nextInstr.mnemonic() == LDA) {
				dp_register a, b;
				//dp-only
				if (line->operands[0]->is_register(a) && line->operands[0]->is_register(b)) {
					if (a == b) {
						line->next = next ? next->next : nullptr;
						delete next;
						continue;
					}
				}

			}
		}

		prev = line;
		line = next;
	}

}
#endif



void print(Line *line) {

	while (line) {

		if (line->label) { printf("%s:\n", line->label->c_str()); }

		if (line->instruction) {
			std::string op;
			AddressMode mode = line->mode;

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
				line->instruction.toString(), 
				op.c_str()
			);
		}

		line = line->next;
	}
}

int main(int argc, char **argv) {

	int c;
	while ((c = getopt(argc, argv, "")) != -1) {


	}
	argv += optind;
	argc -= optind;

	for (int i = 0 ; i < argc; ++i) {
		Line *line = parse_file(argv[i]);
		if (line) {
			simplify(line);
			print(line);
			line = peephole(line);
			print(line);
		}
	}

	return 0;
}

