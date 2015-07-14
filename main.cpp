
#include <string>

#include <cstdio>
#include <cstring>

#include <unistd.h>
#include <fcntl.h>
#include <sysexits.h>
#include <stdlib.h>
#include <sys/xattr.h>

#include "common.h"
#include "Instruction.h"
#include "Machine.h"
#include "Expression.h"

#include "omf.h"
#include "unordered_set"

#include "cxx/filesystem.h"

namespace fs = filesystem;

struct {

	bool S = false;
	bool v = false;
	std::string o;

} flags;

extern OMF::Segment data_to_omf(Segment *segment);
extern OMF::Segment code_to_omf(Segment *segment);

int set_ftype(int fd, uint16_t fileType, uint32_t auxType) {

	int ok;
	uint8_t finfo[32];
	uint8_t ft[1];
	uint8_t at[2];

	ft[0] = fileType & 0xff;
	at[0] = (auxType >> 0) & 0xff;
	at[1] = (auxType >> 8) & 0xff;

	std::fill(std::begin(finfo), std::end(finfo), 0);
	finfo[0] = 'p';
	finfo[1] = fileType & 0xff;
	finfo[2] = (auxType >> 8) & 0xff;
	finfo[3] = (auxType >> 0) & 0xff;
	finfo[4] = 'p';
	finfo[5] = 'd';
	finfo[6] = 'o';
	finfo[7] = 's';

#ifdef __APPLE__
	ok = fsetxattr(fd, XATTR_FINDERINFO_NAME, &finfo, sizeof(finfo), 0, 0);
	if (ok < 0) return ok;

	ok = fsetxattr(fd, "prodos.FileType", &ft, sizeof(ft), 0, 0);
	if (ok < 0) return ok;

	ok = fsetxattr(fd, "prodos.AuxType", &at, sizeof(at), 0, 0);
	if (ok < 0) return ok;
#endif

#ifdef __linux__

#endif

	return 0;
}

void simplify(LineQueue &lines) {

	for (BasicLine *line : lines) {
		for (auto &e : line->operands)
			if (e) e = e->simplify();

	}
}


void print(FILE *file, const LineQueue &lines) {

	for (const BasicLine *line : lines) {

		if (line->label) { fprintf(file, "%s\n", line->label->c_str()); }

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
			fprintf(file, "    %s %s\n", name, s.c_str());
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
				case zp_indirect_z:
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
				case zp_indirect_z:
					op.append("),z");
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


			fprintf(file, "    %s %s\n", 
				line->opcode.toString(), 
				op.c_str()
			);
		}
	}
}

void print(FILE *file, const Segment *segment) {

	const char *start = "start";
	if (segment->convention == Segment::data)
		start = "data";

	fprintf(file, "%s    %s\n", segment->name ? segment->name->c_str() : "", start);
	print(file, segment->lines);
	fprintf(file, "    end\n\n");
}


void process_segments(SegmentQueue segments, fs::path &outfile) {

	int segnum = 1;
	FILE *f = nullptr;
	int fd = -1;


	if (flags.S) {
		if (outfile.empty()) f = stdout;
		else {
			f = fopen(outfile.c_str(), "w");
			if (!f) {
				fprintf(stderr, "Unable to open %s : %s\n", outfile.c_str(), strerror(errno));
				exit(EX_CANTCREAT);
			}
		}
	} else {
		if (outfile.empty()) outfile = "object.omf";

		fd = open(outfile.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0666);
		if (fd < 0) {
				fprintf(stderr, "Unable to open %s : %s\n", outfile.c_str(), strerror(errno));
				exit(EX_CANTCREAT);
		}
	}



	for (auto &seg : segments) {

		if (seg->convention == Segment::data) {

			if (flags.S) {
				// -S -- output code.
				print(f, seg.get());
				continue;
			}

			auto omf = data_to_omf(seg.get());
			omf.segnum = segnum++;
			omf.write(fd);
			continue;
		}

		//auto &lines = seg->lines;
		basic_block(seg.get());

		if (flags.S) {
			// -S -- output code.
			print(f, seg.get());
			continue;
		}

		auto omf = code_to_omf(seg.get());
		omf.segnum = segnum++;
		omf.write(fd);

	}
	if (fd >= 0) {
		set_ftype(fd, 0xb1, 0x0000);
		close(fd);
	}
	if (f && f != stdout) fclose(f);
}



void help() {


}


int main(int argc, char **argv) {

	int c;
	while ((c = getopt(argc, argv, "hvSVo:")) != -1) {
		switch(c) {

			case 'S': // output source code
				flags.S = 1;
				break;

			case 'o':
				flags.o = optarg;
				break;


			case 'h':
				help();
				exit(0);
				break;

			case 'v':
				flags.v = true;
				break;

			case 'V':
				fprintf(stdout, "Version 0.0\n");
				exit(0);
				break;

			case ':':
			case '?':
				help();
				exit(EX_USAGE);
				break;
		}

	}
	argv += optind;
	argc -= optind;

	if (argc == 0) {

		extern bool parse_file(FILE *file, SegmentQueue &rv);

		SegmentQueue segments;

		bool ok = parse_file(stdin, segments);
		if (ok) {

			// ok if ofn is empty.
			fs::path ofn  = std::move(flags.o);
			flags.o.clear();

			process_segments(std::move(segments), ofn);
		}

		if (!ok) exit(EX_DATAERR);
	}

	for (int i = 0 ; i < argc; ++i) {

		SegmentQueue segments;

		bool ok = parse_file(argv[i], segments);
		if (ok) {

			fs::path ifn = argv[i];
			fs::path ofn = std::move(flags.o);
			flags.o.clear();

			if (ofn.empty() && !flags.S) {
				ofn = ifn.filename(); // use cwd.
				//if (flags.S) ofn.replace_extension(".s");
				ofn.replace_extension(".omf");
			}

			process_segments(std::move(segments), ofn);
		}
		if (!ok) exit(EX_DATAERR);
	}

	return 0;
}

