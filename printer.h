#ifndef __printer_h__
#define __printer_h__

#include <stdio.h>
#include "types.h"
#include "common.h"

class printer { 

public:
	virtual ~printer();

	void print(FILE *f, const Module &m);

	virtual void begin(FILE *f);
	virtual void end(FILE *f);

	virtual void print_label(FILE *file, const BasicLinePtr &line);
	virtual void print_opcode(FILE *file, const BasicLinePtr &line);
	virtual void print_data(FILE *f, const BasicLinePtr &line) = 0;
	virtual void begin_segment(FILE *f, const SegmentPtr &segment);
	virtual void end_segment(FILE *f, const SegmentPtr &segment);
};

class orca_printer : public printer {
public:
	virtual void begin(FILE *f);
	virtual void print_data(FILE *f, const BasicLinePtr &line);
	virtual void begin_segment(FILE *f, const SegmentPtr &segment);
	virtual void end_segment(FILE *f, const SegmentPtr &segment);
	virtual void print_label(FILE *file, const BasicLinePtr &line);
};

class mpw_printer : public printer {
public:
	virtual void begin(FILE *f);
	virtual void end(FILE *f);
	virtual void print_data(FILE *f, const BasicLinePtr &line);
	virtual void begin_segment(FILE *f, const SegmentPtr &segment);
	virtual void end_segment(FILE *f, const SegmentPtr &segment);
};


class harpoon_printer : public printer {
public:
	virtual void begin(FILE *f);
	virtual void print_data(FILE *f, const BasicLinePtr &line);
	virtual void begin_segment(FILE *f, const SegmentPtr &segment);
	virtual void end_segment(FILE *f, const SegmentPtr &segment);
};


void print(FILE *f, const Module &m, const printer &p);

#endif
