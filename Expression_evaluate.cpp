
#include "Expression_Internal.h"

#include <stdexcept>

using namespace Expression_Internal;

#pragma mark - evaluate

#if 0
bool Expression::evaluate(uint32_t pc, 
	const identifier_map &env,
	uint32_t &result) const {
	return false;
}

bool IntegerExpression::evaluate(uint32_t pc, 
	const identifier_map &env,
	uint32_t &result) const {
	result = _value;
	return true;
}

bool PCExpression::evaluate(uint32_t pc, 
	const identifier_map &env,
	uint32_t &result) const {
	result = pc;
	return true;
}

bool IdentifierExpression::evaluate(uint32_t pc, 
	const identifier_map &env,
	uint32_t &result) const {

	auto iter = env.find(_value);
	if (iter != env.end()) {
		result = iter->second;
		return true;
	}
	return false;
}


bool UnaryExpression::evaluate(uint32_t pc, 
	const identifier_map &env,
	uint32_t &result) const {

	uint32_t value;
	if (!_children[0]->evaluate(pc, env, value)) return false;

	result = unary_op(_op, value);
	return true;
}


bool BinaryExpression::evaluate(uint32_t pc, 
	const identifier_map &env,
	uint32_t &result) const {

	uint32_t a, b;
	if (!_children[0]->evaluate(pc, env, a)) return false;
	if (!_children[1]->evaluate(pc, env, b)) return false;

	result = binary_op(_op, a, b);
	return true;
}

#endif

#pragma mark - evaluate

bool Expression::evaluate(uint32_t pc,
	const identifier_map &env,
	uint32_t &result) const {

	try {
		result = evaluate(pc, env);
		return true;
	} catch (...) {
		return false;
	}
}


bool Expression::evaluate_relative(uint32_t pc, int32_t &result) const {

	try {
		result = evaluate_relative(pc);
		return true;
	} catch (...) {
		return false;
	}
}

/*
 * these throw a runtime error if unable to calculate an answer.
 *
 */

uint32_t Expression::evaluate(uint32_t pc, const identifier_map &env) const
{
	throw std::runtime_error("unable to evaluate expression");
}

uint32_t PCExpression::evaluate(uint32_t pc, const identifier_map &env) const
{
	return pc;
}

uint32_t RelExpression::evaluate(uint32_t pc, const identifier_map &env) const
{
	return _offset;
}


uint32_t IntegerExpression::evaluate(uint32_t pc, const identifier_map &env) const
{
	return _value;
}


uint32_t IdentifierExpression::evaluate(uint32_t pc, const identifier_map &env) const
{
	auto iter = env.find(_value);
	if (iter == env.end()) {
		std::string msg = "unable to evaluate expression: " + *_value + " not defined";
		throw std::runtime_error(msg);
	}

	return iter->second;
}

uint32_t UnaryExpression::evaluate(uint32_t pc, const identifier_map &env) const
{
	uint32_t a = _children[0]->evaluate(pc, env);

	return unary_op(_op, a);
}

uint32_t BinaryExpression::evaluate(uint32_t pc, const identifier_map &env) const
{
	uint32_t a = _children[0]->evaluate(pc, env);
	uint32_t b = _children[1]->evaluate(pc, env);

	return binary_op(_op, a, b);
}



// bra label 
// make_relative converts to -> bra [__start + pc]
// bra label +- offset will be optimized to a relative.
// all others, leave for the linker to handle.

int32_t Expression::evaluate_relative(uint32_t pc) const
{
	throw std::runtime_error("unable to evaluate expression");
}

int32_t RelExpression::evaluate_relative(uint32_t pc) const {
	return _offset - pc;
}

/*
uint32_t IntegerExpression::evaluate_relative(uint32_t pc) {
	// bra $1234 ? 
}
*/
