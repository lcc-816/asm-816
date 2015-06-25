#ifndef __cookie_h__
#define __cookie_h__

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "common.h"
#include "symbol_table.h"

struct Cookie {

	Line scratch;
	unsigned line_number = 1;
	bool line_error = false;
	bool line_warning = false;

	SegmentQueue segments;
	
	// current segment.
	Segment *segment = nullptr;
	std::unique_ptr<Segment> data_segment;

	LineQueue *lines = nullptr;

	std::unordered_set<const std::string *> export_set;
	std::unordered_set<const std::string *> import_set; // parsed but ignored.

	std::unordered_map<const std::string *, unsigned> labels; // label -> line number.

	symbol_table equates;

	const std::string *current_label; 

	enum {
		none,
		code,
		data
	} seg_type = none;

	// lexer stuff.

	struct {


		int cs;
		int act;
		const char * ts;
		const char * te;

		// for error reporting.
		const char * line_start = nullptr;
		const char * line_end = nullptr;

		int next_operand = 0;


	} fsm;
	
};

#endif
