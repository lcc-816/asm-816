
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


void print(Line *line) {

	while (line) {

		if (line->label) { printf("%s:\n", line->label->c_str()); }

		if (line->instruction) {
			printf("    %s\n", line->instruction.toString());
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
			peephole(line);
			print(line);
		}
	}

	return 0;
}

