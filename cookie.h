#ifndef __cookie_h__
#define __cookie_h__

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "common.h"
#include "symbol_table.h"

struct Cookie {

	Line scratch;
	unsigned line_number;

	SegmentQueue segments;
	
	// current segment.
	Segment *segment = nullptr;
	Segment *data_segment = nullptr;
	std::unordered_set<const std::string *> export_set;

	std::unordered_map<const std::string *, unsigned> labels; // label -> line number.

	symbol_table equates;

	const std::string *current_label; 

	enum {
		none,
		code,
		data
	} seg_type = none;

	LineQueue lines;
};

#endif
