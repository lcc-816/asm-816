#include "register_bits.h"
#include <cstdio>

void register_bits::dump() const {

	printf("%c %c %c %c %c %c %c %c",
		bits & a_mask ? 'a' : '-',
		bits & x_mask ? 'x' : '-',
		bits & y_mask ? 'y' : '-',
		bits & d_mask ? 'd' : '-',
		bits & s_mask ? 's' : '-',
		bits & b_mask ? 'b' : '-',
		bits & b_mask ? 'k' : '-',
		bits & pc_mask ? 'p' : '-'
	);

	printf(" : ");
	printf("%c %c %c %c %c %c %c %c %c",
		bits & (1 << 8 ) ? 'c' : '-',
		bits & (1 << 9 ) ? 'z' : '-',
		bits & (1 << 10) ? 'i' : '-',
		bits & (1 << 11) ? 'd' : '-',
		bits & (1 << 12) ? 'x' : '-',
		bits & (1 << 13) ? 'm' : '-',
		bits & (1 << 14) ? 'v' : '-',
		bits & (1 << 15) ? 'n' : '-',
		bits & (1 << 16) ? 'e' : '-'
	);


	printf("\n");
}