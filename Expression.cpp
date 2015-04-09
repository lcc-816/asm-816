
//#include "Expression.h"

#include <deque>
#include <string>
#include <algorithm>
#include <memory>

#include <cstdio>

#include "Expression.h"

namespace {

	std::deque<Expression *> Pool;

	std::string to_string(uint32_t value)
	{
		char buffer[10] = {0};
		if (value <= 0xff) {
			snprintf(buffer, sizeof(buffer), "$%02x", value);			
		} else if (value <= 0xffff) {
			snprintf(buffer, sizeof(buffer), "$%04x", value);
		} else if (value <= 0xffffff) {
			snprintf(buffer, sizeof(buffer), "$%06x", value);			
		} else {
			snprintf(buffer, sizeof(buffer), "$%08x", value);			
		}
		return buffer;
	}

	std::string to_string(dp_register reg) {
		char buffer[10] = {0};

		snprintf(buffer, sizeof(buffer), "%%%c%u", reg.type, reg.number);
		return buffer;
	}
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
	Expression *e = new Expression(type_variable, value);
	Pool.push_back(e);
	return e;
}

Expression *Expression::String(const std::string *value) {
	Expression *e = new Expression(type_string, value);
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

Expression *Expression::Vector(Expression *child) {
	Expression *e = new Expression(type_vector);

	auto v = new (&e->vector_value) std::vector<Expression *>;
	v->push_back(child);
	// todo -- flatten if child is a vector?

	Pool.push_back(e);
	return e;
}

Expression *Expression::Vector(Expression **children, unsigned count) {
	Expression *e = new Expression(type_vector);

	auto v = new (&e->vector_value) std::vector<Expression *>;
	v->reserve(count);
	for (unsigned i = 0; i < count; ++i) v->push_back(children[i]);
	// todo -- flatten if child is a vector?

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
		case type_vector:
			return simplify_vector();
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

Expression *Expression::simplify_vector() {
	std::vector<Expression *> tmp(vector_value);

	bool delta = false;

	std::transform(tmp.begin(), tmp.end(), tmp.begin(), [&delta](Expression *e){

		Expression *tmp = e->clone();
		if (tmp != e) delta = true;
		return tmp;
	});

	if (!delta) return this;
	return Vector(tmp.data(), tmp.size());
}

#pragma mark - clone

Expression *Expression::clone() {

	switch (_type) {
		default: return this;
		case type_binary: return clone_binary();
		case type_unary: return clone_unary();
		case type_vector: return clone_vector();
	}
}

Expression *Expression::clone_unary() {
	return Expression::Unary(op, children[0]->clone());
}

Expression *Expression::clone_binary() {
	return Expression::Binary(op, children[0]->clone(), children[1]->clone());
}

Expression *Expression::clone_vector() {
	std::vector<Expression *> tmp(vector_value);

	std::transform(tmp.begin(), tmp.end(), tmp.begin(), [](Expression *e) {
		return e->clone();
	});

	return Expression::Vector(tmp.data(), tmp.size());
}


std::string Expression::to_string() const {

	switch(_type)
	{
		case type_binary:
			return to_string_binary();
		case type_unary:
			return to_string_unary();
		case type_pc:
			return "*";
		case type_integer:
			return ::to_string(int_value);
		case type_variable:
			return *string_value;
		case type_register:
			return ::to_string(register_value);
		case type_vector:
			return to_string_vector();
		case type_string:
			{
				std::string s;
				s.push_back('"');
				s.append(*string_value);
				s.push_back('"');
				return s;
			}

		default:
			return "";
	}

}

std::string Expression::to_string_unary() const {
	std::string rv;

	rv.push_back(op);
	rv.append(children[0]->to_string());

	return rv;
}

std::string Expression::to_string_binary() const {
	std::string rv;
	
	// doesn't handle () precedence. 
	// (but neither does the parser)
	rv.append(children[0]->to_string());
	rv.push_back(op);
	rv.append(children[1]->to_string());

	return rv;
}

std::string Expression::to_string_vector() const {
	std::string rv;

	bool first = true;
	for (Expression *e : vector_value) {
		if (!first) { rv.push_back(','); }
		rv.append(e->to_string());
		first = false;	
	}

	return rv;
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

Expression *Expression::append(Expression *child) {
	if (child == this) return this;

	if (_type == type_vector) {
		// flatten if child is a vector?
		vector_value.push_back(child);
		return this;
	}
	Expression *children[2] = {this, child};
	return Expression::Vector(children, 2);
}

#pragma mark - collect_variables

std::vector<const std::string *> Expression::collect_variables() {
	std::vector<const std::string *> rv;
	collect_variables(rv);
	return rv;
}


void Expression::collect_variables(std::vector<const std::string *> &v) {
	switch(_type) {
		case type_unary: collect_variables_unary(v); break;
		case type_binary: collect_variables_binary(v); break;
		case type_vector: collect_variables_vector(v); break;
		case type_variable: v.push_back(string_value); break;
		default: break;
	}
}

void Expression::collect_variables_unary(std::vector<const std::string *> &v) {
	children[0]->collect_variables(v);
}

void Expression::collect_variables_binary(std::vector<const std::string *> &v) {
	children[0]->collect_variables(v);
	children[1]->collect_variables(v);
}

void Expression::collect_variables_vector(std::vector<const std::string *> &v) {
	for (Expression *e : vector_value)
		e->collect_variables(v);
}


