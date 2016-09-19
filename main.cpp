
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
#include "intern.h"

#include "printer.h"

namespace fs = filesystem;

struct {

	bool S = false;
	bool v = false;
	int O = 0;
	unsigned f = 0;
	fs::path o;

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

#ifdef __sun__
	ok = openat(fd, "com.apple.FinderInfo", O_XATTR | O_WRONLY | O_CREAT | O_TRUNC);
	if (ok < 0) return ok;
	write(ok, &finfo, sizeof(finfo));
	close(ok);

	ok = openat(fd, "prodos.FileType", O_XATTR | O_WRONLY | O_CREAT | O_TRUNC);
	if (ok < 0) return ok;
	write(ok, &ft, sizeof(ft));
	close(ok);

	ok = openat(fd, "prodos.AuxType", O_XATTR | O_WRONLY | O_CREAT | O_TRUNC);
	if (ok < 0) return ok;
	write(ok, &at, sizeof(at));
	close(ok);
#endif

#ifdef __linux__

#endif

	return 0;
}

void simplify(LineQueue &lines) {

	for (BasicLinePtr line : lines) {
		for (auto &e : line->operands)
			if (e) e = e->simplify();
	}
}


void process_segments(Module &m, fs::path &outfile) {

	int segnum = 1;
	int fd = -1;

	if (!flags.S) {
		if (outfile.empty()) outfile = "object.omf";

		fd = open(outfile.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0666);
		if (fd < 0) {
				fprintf(stderr, "Unable to open %s : %s\n", outfile.c_str(), strerror(errno));
				exit(EX_CANTCREAT);
		}
	}



	for (auto &seg : m.segments) {

		if (seg->convention == Segment::data) {

			if (flags.S) continue;

			auto omf = data_to_omf(seg.get());
			omf.segnum = segnum++;
			omf.write(fd);
			continue;
		}

		//auto &lines = seg->lines;
		basic_block(seg.get());

		if (flags.S) continue;

		auto omf = code_to_omf(seg.get());
		omf.segnum = segnum++;
		omf.write(fd);

	}
	if (fd >= 0) {
		set_ftype(fd, 0xb1, 0x0000);
		close(fd);
	}
	if (flags.S) {

		printer *p = nullptr;
		switch(flags.f){
			case 1: { static merlin_printer pp; p = &pp; break; }
			case 2: { static mpw_printer pp; p = &pp; break; }
			case 3: { static orca_printer pp; p = &pp; break; }
			default: { static harpoon_printer pp; p = &pp; break; }
		}


		FILE *f = nullptr;
		if (outfile.empty() || outfile == "-") f = stdout;
		else {
			f = fopen(outfile.c_str(), "w");
			if (!f) {
				fprintf(stderr, "Unable to open %s : %s\n", outfile.c_str(), strerror(errno));
				exit(EX_CANTCREAT);
			}
		}

		p->print(f, m);
		if (f != stdout) fclose(f);
	}
}



void help() {


}


int main(int argc, char **argv) {

	std::vector<fs::path> _I;

	int c;
	while ((c = getopt(argc, argv, "hf:I:vSVo:O:")) != -1) {
		switch(c) {

			case 'I': // include directories.
				_I.emplace_back(optarg);
				break;

			case 'S': // output source code
				flags.S = true;
				break;

			case 'o':
				flags.o = optarg;
				break;

			case 'O':
				{
					char c = *optarg;
					if (c >= '0' && c <= '3') flags.O = c - '0';
				}
				break;

			case 'h':
				help();
				exit(0);
				break;

			case 'f':
				{
					std::string tmp(optarg);
					if (tmp == "merlin") flags.f = 1;
					else if (tmp == "mpw") flags.f = 2;
					else if (tmp == "orca") flags.f = 3;
					else {
						fprintf(stderr, "%s: unsupported format\n", optarg);
					}
				}
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

	if (flags.o == "-") {
		flags.S = true;
		flags.o.clear();
	}

	//atexit(clear_intern_table);

	if (argc == 0) {

		auto m = parse_file(STDIN_FILENO);
		if (!m) exit(EX_DATAERR);

		if (m) {

			// ok if ofn is empty.
			fs::path ofn  = std::move(flags.o);
			flags.o.clear();

			process_segments(*m, ofn);
		}
	}

	for (int i = 0 ; i < argc; ++i) {

		fs::path ifn = argv[i];

		auto m = parse_file(ifn);
		if (!m) exit(EX_DATAERR);

		if (m) {

			fs::path ofn = std::move(flags.o);
			flags.o.clear();

			if (ofn.empty() && !flags.S) {
				ofn = ifn.filename(); // use cwd.
				//if (flags.S) ofn.replace_extension(".s");
				ofn.replace_extension(".omf");
			}

			process_segments(*m, ofn);
		}
	}

	return 0;
}

