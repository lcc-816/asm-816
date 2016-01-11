#ifndef __types_h__
#define __types_h__

#include <memory>
#include <stdint.h>

// define ptr types.


typedef const std::string *identifier;


typedef std::shared_ptr<class Expression> ExpressionPtr;
typedef std::shared_ptr<class VectorExpression> VectorExpressionPtr;
typedef std::shared_ptr<struct BasicLine> BasicLinePtr;
typedef std::shared_ptr<class BasicBlock> BasicBlockPtr;

typedef std::unique_ptr<class Segment> SegmentPtr;

#endif
