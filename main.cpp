
#include <string>

#include <cstdio>
#include <unistd.h>

bool parse_file(const std::string &filename);

int main(int argc, char **argv) {

	int c;
	while ((c = getopt(argc, argv, "")) != -1) {


	}
	argv += optind;
	argc -= optind;

	for (int i = 0 ; i < argc; ++i) {
		parse_file(argv[i]);
	}

	return 0;
}