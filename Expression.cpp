

#include <deque>
#include <string>
#include <algorithm>
#include <memory>

#include <cstdio>

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

#pragma mark - class definitions

class PCExpression : public Expression {
public:
	PCExpression() : Expression(type_pc)
	{}

	virtual void to_string(std::string &) const final;
};


class IntegerExpression : public Expression {
public:
	IntegerExpression(uint32_t value) : 
	Expression(type_integer), _value(value)
	{}

	virtual bool is_integer(uint32_t &) const final;
	virtual void to_string(std::string &) const final;

private:
	uint32_t _value;
};




class RegisterExpression : public Expression {
public:
	RegisterExpression(dp_register value) : 
	Expression(type_register), _value(value)
	{}

	virtual bool is_register(dp_register &) const final;
	virtual void to_string(std::string &) const final;
	virtual void rename(dp_register, dp_register) final;

private:
	dp_register _value;
};




class IdentifierExpression : public Expression {
public:
	IdentifierExpression(identifier value) : Expression(type_identifier), _value(value)
	{}

	virtual bool is_identifier(identifier &) const final;
	virtual void to_string(std::string &) const final;

	virtual void identifiers(std::vector<identifier> &) const final;
	virtual void rename(identifier, identifier) final;	

private:
	identifier _value;
};


class StringExpression : public Expression {
public:
	StringExpression(identifier value) : Expression(type_string), _value(value)
	{}

	virtual bool is_string(identifier &) const final;
	virtual void to_string(std::string &) const final;

private:
	identifier _value;
};



#if 0
	// in header now.
class VectorExpression : public Expression {
public:
	VectorExpression() : Expression(type_vector)
	{}

	VectorExpression(std::vector<ExpressionPtr> &&v) : 
	Expression(type_vector), _values(std::move(v))
	{}

	template <class T>
	VectorExpression(T<ExpressionPtr> &v) : Expression(type_vector), _values(v)
	{}

	virtual void to_string(std::string &) const final;
	virtual bool is_vector(std::vector<ExpressionPtr> &) const final;

	virtual void rename(identifier, identifier) final;
	virtual void rename(dp_register, dp_register) final;
	virtual ExpressionPtr simplify() final;

	virtual ExpressionPtr append(ExpressionPtr child) final;

private:
	std::vector<ExpressionPtr> _values;
};
#endif



class UnaryExpression : public Expression {
public:
	UnaryExpression(unsigned op, ExpressionPtr a) :
	Expression(type_unary), _op(op), _children{a}
	{}

	virtual void to_string(std::string &) const final;
	virtual void identifiers(std::vector<identifier> &) const final;

	virtual void rename(identifier, identifier) final;
	virtual void rename(dp_register, dp_register) final;
	virtual ExpressionPtr simplify() final;

		
private:
	unsigned _op;
	std::array<ExpressionPtr, 1> _children;
};



class BinaryExpression : public Expression {
public:
	BinaryExpression(unsigned op, ExpressionPtr a, ExpressionPtr b) :
	Expression(type_binary), _op(op), _children{a, b}
	{}

	virtual void to_string(std::string &) const final;
	virtual void identifiers(std::vector<identifier> &rv) const final;

	virtual void rename(identifier, identifier) final;
	virtual void rename(dp_register, dp_register) final;
	virtual ExpressionPtr simplify() final;
	
private:
	unsigned _op;
	std::array<ExpressionPtr, 2> _children;
};

#pragma mark - destructors

Expression::~Expression() {
}

VectorExpression::~VectorExpression() {
}




#pragma mark - constructors
Expression *Expression::PC() {
	static ExpressionPtr pc = new PCExpression();

	return pc;
}

Expression *Expression::Integer(uint32_t value) {
	ExpressionPtr e = new IntegerExpression(value);
	Pool.push_back(e);
	return e;
}

Expression *Expression::Register(dp_register value) {
	ExpressionPtr e = new RegisterExpression(value);
	Pool.push_back(e);
	return e;
}

Expression *Expression::Identifier(const std::string *value) {
	ExpressionPtr e = new IdentifierExpression(value);
	Pool.push_back(e);
	return e;
}

Expression *Expression::String(const std::string *value) {
	ExpressionPtr e = new StringExpression(value);
	Pool.push_back(e);
	return e;
}


Expression *Expression::Unary(unsigned op, ExpressionPtr child) {
	ExpressionPtr e = new UnaryExpression(op, child);

	Pool.push_back(e);
	return e;
}

ExpressionPtr Expression::Binary(unsigned op, ExpressionPtr a, ExpressionPtr b) {
	ExpressionPtr e = new BinaryExpression(op, a, b);

	Pool.push_back(e);
	return e;
}

VectorExpressionPtr Expression::Vector() {
	VectorExpressionPtr e = new VectorExpression();

	Pool.push_back(e);
	return e;
}




#pragma mark - is_xxx

bool Expression::is_integer(uint32_t &) const {
	return false;
}

bool Expression::is_register(dp_register &) const {
	return false;
}

bool Expression::is_identifier(identifier &) const {
	return false;
}

bool Expression::is_vector(std::vector<ExpressionPtr> &) const {
	return false;
}

bool Expression::is_string(const std::string *&) const {
	return false;
}


bool IntegerExpression::is_integer(uint32_t &rv) const {
	rv = _value;
	return true;
}

bool RegisterExpression::is_register(dp_register &rv) const {
	rv = _value;
	return true;
}

bool IdentifierExpression::is_identifier(identifier &rv) const {
	rv = _value;
	return true;
}

bool StringExpression::is_string(const std::string *&rv) const {
	rv = _value;
	return true;
}

bool VectorExpression::is_vector(std::vector<ExpressionPtr> &rv) const {
	rv = _children;
	return true;
}




#pragma mark - to_string

void PCExpression::to_string(std::string &rv) const {
	rv.push_back('*');
}

void IntegerExpression::to_string(std::string &rv) const {
	char buffer[10];
	if (_value <= 0xff) snprintf(buffer, sizeof(buffer), "%02x", _value);
	else if (_value <= 0xffff) snprintf(buffer, sizeof(buffer), "%04x", _value);
	else snprintf(buffer, sizeof(buffer), "%08x", _value);

	rv.push_back('$');
	rv.append(buffer);
}


void RegisterExpression::to_string(std::string &rv) const {
	char buffer[10];
	snprintf(buffer, sizeof(buffer), "%u", _value.number);
	rv.push_back('%');
	rv.push_back(_value.type);
	rv.append(buffer);
}

void IdentifierExpression::to_string(std::string &rv) const {
	rv.append(*_value);
}

void UnaryExpression::to_string(std::string &rv) const {
	rv.push_back(_op);
	_children[0]->to_string(rv);
}

void BinaryExpression::to_string(std::string &rv) const {
	_children[0]->to_string(rv);
	rv.push_back(_op);
	_children[1]->to_string(rv);
}

void VectorExpression::to_string(std::string &rv) const {
	bool first = true;
	for (ExpressionPtr e : _children) {
		if (first) first = false;
		else rv.push_back(',');
		e->to_string(rv);
	}
}

void StringExpression::to_string(std::string &rv) const {
    // todo -- quote it...
    rv.push_back('"');
    rv.append(*_value);
    rv.push_back('"');
}


#pragma mark - simplify

ExpressionPtr Expression::simplify() {
    return this;
}

ExpressionPtr UnaryExpression::simplify() {
	uint32_t value;

	std::transform(_children.begin(), _children.end(), _children.begin(), [](ExpressionPtr e){
		return e->simplify();
	});


	if (_children[0]->is_integer(value)) {
		switch(_op) {
		case '^': value = value >> 16;	break;
		case '!': value = !value; break;
		case '+': value = +value; break;
		case '-': value = -value; break;
		case '~': value = ~value; break;
		}
		return Expression::Integer(value);
	}
	return this;
}


ExpressionPtr BinaryExpression::simplify() {
	uint32_t a,b;

	std::transform(_children.begin(), _children.end(), _children.begin(), [](ExpressionPtr e){
		return e->simplify();
	});
    
	if (_children[0]->is_integer(a) && _children[1]->is_integer(b)) {
	    uint32_t value = 0;
		switch(_op) {
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
	
	// shortcut logical expressions...
	if (_children[0]->is_integer()) {
		switch(_op) {
		case '||': 
			if (a) return Expression::Integer(1);
			break;
		case '&&':
			if (!a) return Expression::Integer(0);
			break;
		}
	}

	if (_children[1]->is_integer()) {
		switch(_op) {
		case '||': 
			if (b) return Expression::Integer(1);
			break;
		case '&&':
			if (!b) return Expression::Integer(0);
			break;
		}
	}

	return this;
}

ExpressionPtr VectorExpression::simplify() {

	std::transform(_children.begin(), _children.end(), _children.begin(), [](ExpressionPtr e){
		return e->simplify();
	});
	
	return this;
}

#pragma mark - rename

void Expression::rename(dp_register, dp_register) {
}

void Expression::rename(identifier, identifier) {
}

void RegisterExpression::rename(dp_register a, dp_register b) {
	if (_value == a) _value = b;
}

void IdentifierExpression::rename(identifier a, identifier b) {
	if (_value == a) _value = b;
}

void VectorExpression::rename(dp_register a, dp_register b) {
	for (auto e: _children) e->rename(a, b);
}

void VectorExpression::rename(identifier a, identifier b) {
	for (auto e: _children) e->rename(a, b);
}

void UnaryExpression::rename(dp_register a, dp_register b) {
	for (auto e: _children) e->rename(a, b);
}

void UnaryExpression::rename(identifier a, identifier b) {
	for (auto e: _children) e->rename(a, b);
}

void BinaryExpression::rename(dp_register a, dp_register b) {
	for (auto e: _children) e->rename(a, b);
}

void BinaryExpression::rename(identifier a, identifier b) {
	for (auto e: _children) e->rename(a, b);
}

#pragma mark - identifiers


void Expression::identifiers(std::vector<identifier> &rv) const {
}

void IdentifierExpression::identifiers(std::vector<identifier> &rv) const {
	rv.push_back(_value);
}

void UnaryExpression::identifiers(std::vector<identifier> &rv) const {
	for (auto e: _children) e->identifiers(rv);

}

void BinaryExpression::identifiers(std::vector<identifier> &rv) const {
	for (auto e: _children) e->identifiers(rv);
}

void VectorExpression::identifiers(std::vector<identifier> &rv) const {
	for (auto e: _children) e->identifiers(rv);
}
