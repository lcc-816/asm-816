
#include <string>
#include <unordered_set>

#include <cstdio>
#include <cstring>

#include <unistd.h>
#include <fcntl.h>
#include <sysexits.h>
#include <stdlib.h>
#include <err.h>

#include "common.h"
#include "Instruction.h"
#include "Machine.h"
#include "Expression.h"

#include "omf.h"

#include "cxx/filesystem.h"
#include "intern.h"

#include "printer.h"

#include "finder_info_helper.h"

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

bool set_ftype(const filesystem::path &name, uint16_t fileType, uint32_t auxType) {

	finder_info_helper fi;
	std::error_code ec;

	if (!fi.open(name, ec, finder_info_helper::read_write)) {
		warnx("Unable to set file type for %s: %s", name.c_str(), ec.message().c_str());
		return false;
	}
	fi.set_prodos_file_type(fileType, auxType);
	if (!fi.write(ec)) {
		warnx("Unable to set file type for %s: %s", name.c_str(), ec.message().c_str());
		return false;
	}
	return true;
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
			err(EX_CANTCREAT, "Unable to open %s", outfile.c_str());
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
		set_ftype(outfile, 0xb1, 0x0000);
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
				err(EX_CANTCREAT, "Unable to open %s", outfile.c_str());
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

