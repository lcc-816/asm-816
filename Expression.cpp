
//#include "Expression.h"

#include <deque>
#include <string>
#include <algorithm>
#include <memory>

#include "Expression.h"

namespace {

	std::deque<Expression *> Pool;

}


void Expression::ErasePool(void) {

	for(Expression *e : Pool) {
		delete e;
	}

	Pool.clear();
}


Expression *Expression::PC() {
	static Expression pc(type_pc);

	return &pc;
}

Expression *Expression::Integer(uint32_t value) {
	Expression *e = new Expression(value);
	Pool.push_back(e);
	return e;
}

Expression *Expression::Register(dp_register value) {
	Expression *e = new Expression(value);
	Pool.push_back(e);
	return e;
}

Expression *Expression::Variable(const std::string *value) {
	Expression *e = new Expression(value);
	Pool.push_back(e);
	return e;
}

Expression *Expression::Unary(unsigned op, Expression *child) {
	Expression *e = new Expression(op, child);

	Pool.push_back(e);
	return e;
}

Expression *Expression::Binary(unsigned op, Expression *a, Expression *b) {
	Expression *e = new Expression(op, a, b);

	Pool.push_back(e);
	return e;
}


Expression *Expression::simplify() {
	switch(_type) {
		default:
			return this;
		case type_unary:
			return simplify_unary();
		case type_binary:
			return simplify_binary();
	}
}

Expression *Expression::simplify_unary() {
	Expression *child = children[0]->simplify();
	children[0] = child;
	uint32_t value;

	if (child->is_integer(value)) {
		switch (op) {
			case '+': return child;
			case '-': value = -value; break;
			case '^': value = value >> 16; break;
			case '!': value = !value; break;
			case '~': value = ~value; break;
		}
		return Expression::Integer(value);
	}
	return this;
}

Expression *Expression::simplify_binary() {
	Expression *lhs = children[0]->simplify();
	Expression *rhs = children[1]->simplify();

	children[0] = lhs;
	children[1] = rhs;

	uint32_t a, b;

	if (lhs->is_integer(a) && rhs->is_integer(b)) {
		uint32_t value = 0;
		switch (op) {
			case '+': value = a + b; break;
			case '-': value = a - b; break;
			case '*': value = a * b; break;
			case '/': value = a / b; break;
			case '%': value = a % b; break;
			case '^': value = a ^ b; break;
			case '|': value = a | b; break;
			case '&': value = a & b; break;
			case '>>': value = a >> b; break;
			case '<<': value = a << b; break;
			case '&&': value = a && b; break;
			case '||': value = a || b; break;
		}
		return Expression::Integer(value);
	}
	return this;
}

#pragma mark - clone

Expression *Expression::clone() {

	switch (_type) {
		default: return this;
		case type_binary: return clone_binary();
		case type_unary: return clone_unary();
	}
}

Expression *Expression::clone_unary() {
	return Expression::Unary(op, children[0]->clone());
}

Expression *Expression::clone_binary() {
	return Expression::Binary(op, children[0]->clone(), children[1]->clone());
}

#if 0
Expression *Expression::evaluate(uint32_t pc, const varmap &map) {

	switch(_op) {
		default:
			return *this;
		case type_binary:
			return evaluate_binary(pc, map);
		case type_unary:
			return evaluate_unary(pc, map);
		case type_pc:
			return Expression::Integer(pc);
		case type_variable:
			{
				auto iter = map.find(s);
				if (iter == map.end()) return this;
				return Integer(iter->second);
			}
	}
}



# pragma mark - ==

bool Expression::operator==(const Expression &other) {
	// hmmm.. should * == * ? only true if same line.
	if (type == type_pc) return false;

	if (this == &other) return true;
	if (type != other->type) return false;

	switch (type) {
		case type_variable:
			return string_value == other->string_value;
		case type_integer:
			return int_value == other->int_value;
		case type_unary:
			return op == other->op 
				&& (*children[0] == *other->children[0]);
		case type_binary:
			return op == other->op 
				&& (*children[0] == *other->children[0]);
				&& (*children[1] == *other->children[1]);

		default:
			return false;
	}
}

#endif