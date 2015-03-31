
//#include "Expression.h"

#include <dequeue>
#include <string>
#include <algorithm>
#include <memory>

namespace {

	std::dequeue<Expression *> Pool;

}


class Expression {

public:


	Expression *simplify();
	Expression *evaluate(uint32_t pc, const std::unordered_map<std::string, uint32_t> &map);
	Expression *clone();

	bool operator==(const Expression &other);
	bool operator!=(const Expression &other);

	~Expression();
private:

	unsigned type = 0;
	Expression *children[2] = {0, 0};

	union {
		uint32_t int_value = 0;
		uint32_t op = 0;
		std::string *string_value;
	};


	enum {
		type_integer,
		type_unary,
		type_binary,
		type_variable,
		type_pc,
		type_register
	}

	Expression(unsigned t) type(t) {}

	Expression *simplify_binary();
	Expression *simplify_unary();
};

void Expression::ClearPool(void) {

	for(Expression e : Pool) {
		e->delete();
	}

	Pool.clear();
}

Expression::~Expression()
{
}
Expression *Expression::PC() {
	static Expression pc(type_pc);

	return &pc;
}

Expression *Expression::Integer(uint32_t value) {
	std::unique_ptr<IntegerExpression> e = std::make_unique(type_integer);
	auto ptr = e->get();

	Arena.emplace_back(e);
	return ptr;
}

Expression *Expression::Unary(unsigned op, Expression *child) {
	std::unique_ptr<Expression> e = std::make_unique(type_unary);
	auto ptr = e->get();
	ptr->op = op;
	ptr->child[0] = child;

	Arena.emplace_back(e);
	return ptr;
}

Expression *Expression::simplify() {
	switch(_type) {
		default:
			return this;
		case type_unary:
			return simplify_unary();
		case type_binary():
			return simplify_binary();
	}
}

Expression *Expression::simplify_unary() {
	child = children[0]->simplify();
	children[0] = child;
	if (child->type() == type_integer()) {
		uint32_t value = child->int_value();
		switch (_op) {
			case '+': break;
			case '-': value = -value; break;
			case '^': value = value >> 16; break;
		}
		return Expression::Integer(value);
	}
	return this;
}

Expression::simplify_binary() {
	Expression *lhs = children[0]->simplify();
	Expression *rhs = children[1]->simplify();

	children[0] = lhs;
	children[1] = rhs;
	if (lhs->type == type_integer && rhs->type == type_integer) {
		uint32_t a = lhs.int_value();
		uint32_t b = rhs.int_value();
		uint32_t value = 0;
		switch (_op) {
			case '+': value = a + b; break;
			case '-': value = a - b; break;
		}
		return Expression::Integer(value);
	}
	return this;
}

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

#pragma mark - clone

Expression *Expression::clone() {

	switch (type) {
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