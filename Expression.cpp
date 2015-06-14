

#include <deque>
#include <string>
#include <algorithm>
#include <memory>

#include <cstdio>

#include "Expression.h"




namespace {

	std::deque<Expression *> Pool;


	uint32_t unary_op(unsigned op, uint32_t value) {
		switch(op) {
		case '^': return value >> 16; break;
		case '!': return !value; break;
		case '+': return +value; break;
		case '-': return -value; break;
		case '~': return ~value; break;
		}
		return -1;
	}

	uint32_t binary_op(unsigned op, uint32_t a, uint32_t b) {

		switch(op) {
			case '+': return a + b; break;
			case '-': return a - b; break;
			case '*': return a * b; break;
			case '/': return a / b; break;
			case '%': return a % b; break;
			case '^': return a ^ b; break;
			case '|': return a | b; break;
			case '&': return a & b; break;
			case '>>': return a >> b; break;
			case '<<': return a << b; break;
			case '&&': return a && b; break;
			case '||': return a || b; break;

			case '>=': return a >= b; break;
			case '<=': return a <= b; break;
			case '>': return a > b; break;
			case '<': return a < b; break;
			case '=': return a = b; break;

			case '^^':
				// logical exclusive or.
				a = a ? 1 : 0;
				b = b ? 1 : 0;
				return a ^ b;
		}
		return -1;
	}


}

class BinaryExpression;
class IdentifierExpression;
class IntegerExpression;
class PCExpression;
class RegisterExpression;
class StringExpression;
class UnaryExpression;

class Expression::Visitor {
public:
	virtual ~Visitor();
	virtual void visit(BinaryExpression &);
	virtual void visit(IdentifierExpression &);
	virtual void visit(IntegerExpression &);
	virtual void visit(PCExpression &);
	virtual void visit(RegisterExpression &);
	virtual void visit(StringExpression &);
	virtual void visit(UnaryExpression &);
	virtual void visit(VectorExpression &);

};



class Expression::MapVisitor {
public:
	virtual ~MapVisitor();
	virtual ExpressionPtr visit(BinaryExpression &);
	virtual ExpressionPtr visit(IdentifierExpression &);
	virtual ExpressionPtr visit(IntegerExpression &);
	virtual ExpressionPtr visit(PCExpression &);
	virtual ExpressionPtr visit(RegisterExpression &);
	virtual ExpressionPtr visit(StringExpression &);
	virtual ExpressionPtr visit(UnaryExpression &);
	virtual ExpressionPtr visit(VectorExpression &);

};



void Expression::ErasePool(void) {

	for(ExpressionPtr e : Pool) {
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

	virtual bool evaluate(uint32_t pc, 
		const std::unordered_map<identifier, uint32_t> &env,
		uint32_t &result) const final;

	virtual void to_omf(std::vector<uint8_t> &) const final;

	//virtual void set_pc(uint32_t pc) final;
	virtual void accept(Visitor &) final;
	virtual ExpressionPtr accept(MapVisitor &) final;

private:
		//bool _has_pc = false;
		//uint32_t _pc = 0;
};


class IntegerExpression : public Expression {
public:
	IntegerExpression(uint32_t value) : 
	Expression(type_integer), _value(value)
	{}

	virtual bool is_integer(uint32_t &) const final;
	virtual void to_string(std::string &) const final;

	virtual bool evaluate(uint32_t pc, 
		const std::unordered_map<identifier, uint32_t> &env,
		uint32_t &result) const final;

	virtual void to_omf(std::vector<uint8_t> &) const final;
	virtual void accept(Visitor &) final;
	virtual ExpressionPtr accept(MapVisitor &) final;

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

	//virtual ExpressionPtr simplify(dp_register oldreg, unsigned dp) final;
	virtual void accept(Visitor &) final;
	virtual ExpressionPtr accept(MapVisitor &) final;

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

	virtual bool evaluate(uint32_t pc, 
		const std::unordered_map<identifier, uint32_t> &env,
		uint32_t &result) const final;

	virtual void to_omf(std::vector<uint8_t> &) const final;
	virtual void accept(Visitor &) final;
	virtual ExpressionPtr accept(MapVisitor &) final;

private:
	identifier _value;
};


class StringExpression : public Expression {
public:
	StringExpression(identifier value) : Expression(type_string), _value(value)
	{}

	virtual bool is_string(identifier &) const final;
	virtual void to_string(std::string &) const final;
	virtual void accept(Visitor &) final;
	virtual ExpressionPtr accept(MapVisitor &) final;

private:
	identifier _value;
};



class UnaryExpression : public Expression {
public:
	typedef std::array<ExpressionPtr, 1> children_type;

	UnaryExpression(unsigned op, ExpressionPtr a) :
	Expression(type_unary), _op(op), _children{a}
	{}

	virtual void to_string(std::string &) const final;
	virtual void identifiers(std::vector<identifier> &) const final;

	virtual void rename(identifier, identifier) final;
	virtual void rename(dp_register, dp_register) final;
	virtual ExpressionPtr simplify() final;
	//virtual ExpressionPtr simplify(dp_register oldreg, unsigned dp) final;

	virtual bool evaluate(uint32_t pc, 
		const std::unordered_map<identifier, uint32_t> &env,
		uint32_t &result) const final;

	virtual void to_omf(std::vector<uint8_t> &) const final;

	virtual void accept(Visitor &) final;
	virtual ExpressionPtr accept(MapVisitor &) final;

private:
	unsigned _op;
	children_type _children;
};



class BinaryExpression : public Expression {
public:
	typedef std::array<ExpressionPtr, 2> children_type;

	BinaryExpression(unsigned op, ExpressionPtr a, ExpressionPtr b) :
	Expression(type_binary), _op(op), _children{a, b}
	{}

	virtual void to_string(std::string &) const final;
	virtual void identifiers(std::vector<identifier> &rv) const final;

	virtual void rename(identifier, identifier) final;
	virtual void rename(dp_register, dp_register) final;
	virtual ExpressionPtr simplify() final;
	//virtual ExpressionPtr simplify(dp_register oldreg, unsigned dp) final;

	virtual bool evaluate(uint32_t pc, 
		const std::unordered_map<identifier, uint32_t> &env,
		uint32_t &result) const final;

	virtual void to_omf(std::vector<uint8_t> &) const final;

	virtual void accept(Visitor &) final;
	virtual ExpressionPtr accept(MapVisitor &) final;

private:
	unsigned _op;
	children_type _children;
};

#pragma mark - destructors

Expression::~Expression() {
}

VectorExpression::~VectorExpression() {
}




#pragma mark - constructors
Expression *Expression::PC() {
	static ExpressionPtr pc = new PCExpression();

	//ExpressionPtr e = new PCExpression();
	//Pool.push_back(e);
	//return e;

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

VectorExpressionPtr Expression::Vector(const std::vector<ExpressionPtr> &children) {
	VectorExpressionPtr e = new VectorExpression(children);

	Pool.push_back(e);
	return e;
}

VectorExpressionPtr Expression::Vector(std::vector<ExpressionPtr> &&children) {
	VectorExpressionPtr e = new VectorExpression(std::move(children));

	Pool.push_back(e);
	return e;
}


VectorExpression::VectorExpression(const std::vector<ExpressionPtr> &children) :
	Expression(type_vector), _children(children)
{}

VectorExpression::VectorExpression(std::vector<ExpressionPtr> &&children) :
	Expression(type_vector), _children(std::move(children))
{}



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
		return Expression::Integer(unary_op(_op, value));
	}

	return this;
}


ExpressionPtr BinaryExpression::simplify() {
	uint32_t a,b;

	std::transform(_children.begin(), _children.end(), _children.begin(), [](ExpressionPtr e){
		return e->simplify();
	});
    
	if (_children[0]->is_integer(a) && _children[1]->is_integer(b)) {
		return Expression::Integer(binary_op(_op, a, b));
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

#pragma mark - evaluate

bool Expression::evaluate(uint32_t pc, 
	const std::unordered_map<identifier, uint32_t> &env,
	uint32_t &result) const {
	return false;
}

bool IntegerExpression::evaluate(uint32_t pc, 
	const std::unordered_map<identifier, uint32_t> &env,
	uint32_t &result) const {
	result = _value;
	return true;
}

bool PCExpression::evaluate(uint32_t pc, 
	const std::unordered_map<identifier, uint32_t> &env,
	uint32_t &result) const {
	result = pc;
	return true;
}

bool IdentifierExpression::evaluate(uint32_t pc, 
	const std::unordered_map<identifier, uint32_t> &env,
	uint32_t &result) const {

	auto iter = env.find(_value);
	if (iter != env.end()) {
		result = iter->second;
		return true;
	}
	return false;
}


bool UnaryExpression::evaluate(uint32_t pc, 
	const std::unordered_map<identifier, uint32_t> &env,
	uint32_t &result) const {

	uint32_t value;
	if (!_children[0]->evaluate(pc, env, value)) return false;

	result = unary_op(_op, value);
	return true;
}


bool BinaryExpression::evaluate(uint32_t pc, 
	const std::unordered_map<identifier, uint32_t> &env,
	uint32_t &result) const {

	uint32_t a, b;
	if (!_children[0]->evaluate(pc, env, a)) return false;
	if (!_children[1]->evaluate(pc, env, b)) return false;

	result = binary_op(_op, a, b);
	return true;
}


#pragma mark - simplify (dp)
#if 0
ExpressionPtr Expression::simplify(dp_register oldreg, unsigned dp) {
	return this;
}

ExpressionPtr RegisterExpression::simplify(dp_register oldreg, unsigned dp) {
	if (_value == oldreg) return Expression::Integer(dp);
	return *this;
}
#endif

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


#pragma mark - to_omf

void Expression::to_omf(std::vector<uint8_t> &rv) const {
	throw std::runtime_error("expression not supported by omf");
}

void UnaryExpression::to_omf(std::vector<uint8_t> &rv) const {

	for (auto e : _children) e->to_omf(rv);


	switch(_op) {
		case '^':
			// special case...
			// ^x = x >> 16.
			rv.push_back(0x81);
			rv.push_back((-16u >> 0 ) & 0xff);
			rv.push_back((-16u >> 8 ) & 0xff);
			rv.push_back((-16u >> 16) & 0xff);
			rv.push_back((-16u >> 24) & 0xff);
			rv.push_back(0x07);
			break;
		case '-': rv.push_back(0x06); break;
		case '+': break; // nop
		case '!': rv.push_back(0x0b); break;
		case '~': rv.push_back(0x15); break;
		default:
			throw std::runtime_error("unary operation not supported by omf");
	}
}

void BinaryExpression::to_omf(std::vector<uint8_t> &rv) const {
	uint8_t omf_op;

	std::array<ExpressionPtr, 2> children = _children;

	// sigh... only 1 bit shift operator, use -value for >>
	if (_op == '>>') {
		ExpressionPtr tmp = Expression::Unary('-', _children[1]);
		children[1] = tmp->simplify();
	}

	for (auto e : children) e->to_omf(rv);

	switch(_op) {
		case '+': omf_op = 0x01; break;
		case '-': omf_op = 0x02; break;
		case '*': omf_op = 0x03; break;
		case '/': omf_op = 0x04; break;
		case '%': omf_op = 0x05; break;


		case '>>':
		case '<<': omf_op = 0x07; break;

		case '<=': omf_op = 0x0c; break;
		case '>=': omf_op = 0x0d; break;
		case '!=': omf_op = 0x0e; break;
		case '<': omf_op = 0x0f; break;
		case '>': omf_op = 0x10; break;
		case '=': omf_op = 0x11; break;

		case '&&': omf_op = 0x08; break;
		case '||': omf_op = 0x09; break;
		case '^^': omf_op = 0x0a; break;

		case '&': omf_op = 0x12; break;
		case '|': omf_op = 0x13; break;
		case '^': omf_op = 0x14; break;

		default:
			throw std::runtime_error("binary operation not supported by omf");
	}

	rv.push_back(omf_op);
}

void IntegerExpression::to_omf(std::vector<uint8_t> &rv) const {

	rv.push_back(0x81);
	rv.push_back(_value >> 0 );
	rv.push_back(_value >> 8 );
	rv.push_back(_value >> 16);
	rv.push_back(_value >> 24);
}


void PCExpression::to_omf(std::vector<uint8_t> &rv) const {
#if 0
	if (_pc != -1) {

		// 0x87 = REL, followed by numlen displacement from start of module ("pc/offset")

		rv.push_back(0x87);

		rv.push_back(_pc >> 0 );
		rv.push_back(_pc >> 8 );
		rv.push_back(_pc >> 16);
		rv.push_back(_pc >> 24);

		return;
	}
	#endif
	rv.push_back(0x80); // wrong?  off by operand size?
}

void IdentifierExpression::to_omf(std::vector<uint8_t> &rv) const {

	rv.push_back(0x83);
	rv.push_back(_value->length());

	for (auto c : *_value) rv.push_back(c);

}


#pragma mark - accept

/*
void Expression::accept(Visitor &v) {
	v.visit(*this);
}
*/

void VectorExpression::accept(Visitor &v) {
	v.visit(*this);
	for (auto e : _children) e->accept(v);
}

void UnaryExpression::accept(Visitor &v) {
	v.visit(*this);
	for (auto e : _children) e->accept(v);
}

void BinaryExpression::accept(Visitor &v) {
	v.visit(*this);
	for (auto e : _children) e->accept(v);
}

#undef CREATE_VISITOR
#define CREATE_VISITOR(klass) void klass::accept(Visitor &v) { v.visit(*this); }
CREATE_VISITOR(IdentifierExpression);
CREATE_VISITOR(IntegerExpression);
CREATE_VISITOR(PCExpression);
CREATE_VISITOR(RegisterExpression);
CREATE_VISITOR(StringExpression);

ExpressionPtr UnaryExpression::accept(MapVisitor &v) {
	children_type tmp(_children);

	std::transform(tmp.begin(), tmp.end(), tmp.begin(),
		[&v](ExpressionPtr e) { return e->accept(v); }
	);

	if (tmp == _children) return v.visit(*this);
	auto self = Unary(_op, tmp[0]);
	return v.visit(*(UnaryExpression *)self);
}


ExpressionPtr BinaryExpression::accept(MapVisitor &v) {
	children_type tmp(_children);

	std::transform(tmp.begin(), tmp.end(), tmp.begin(),
		[&v](ExpressionPtr e) { return e->accept(v); }
	);

	if (tmp == _children) return v.visit(*this);
	auto self = Binary(_op, tmp[0], tmp[1]);
	return v.visit(*(BinaryExpression *)self);
}

ExpressionPtr VectorExpression::accept(MapVisitor &v) {
	children_type tmp(_children);

	std::transform(tmp.begin(), tmp.end(), tmp.begin(),
		[&v](ExpressionPtr e) { return e->accept(v); }
	);

	if (tmp == _children) return v.visit(*this);
	auto self = Vector(std::move(tmp));
	return v.visit(*self);
}


#undef CREATE_VISITOR
#define CREATE_VISITOR(klass) ExpressionPtr klass::accept(MapVisitor &v) { return v.visit(*this); }
CREATE_VISITOR(IdentifierExpression);
CREATE_VISITOR(IntegerExpression);
CREATE_VISITOR(PCExpression);
CREATE_VISITOR(RegisterExpression);
CREATE_VISITOR(StringExpression);


#pragma mark - visitors


Expression::Visitor::~Visitor() {}
void Expression::Visitor::visit(BinaryExpression &) {}
void Expression::Visitor::visit(IdentifierExpression &) {}
void Expression::Visitor::visit(IntegerExpression &) {}
void Expression::Visitor::visit(PCExpression &) {}
void Expression::Visitor::visit(RegisterExpression &) {}
void Expression::Visitor::visit(StringExpression &) {}
void Expression::Visitor::visit(UnaryExpression &) {}
void Expression::Visitor::visit(VectorExpression &) {}


Expression::MapVisitor::~MapVisitor() {}
// these should all be const, but that screws up everything.
ExpressionPtr Expression::MapVisitor::visit(BinaryExpression &e) { return &e; }
ExpressionPtr Expression::MapVisitor::visit(IdentifierExpression &e) { return &e; }
ExpressionPtr Expression::MapVisitor::visit(IntegerExpression &e) { return &e; }
ExpressionPtr Expression::MapVisitor::visit(PCExpression &e) { return &e; }
ExpressionPtr Expression::MapVisitor::visit(RegisterExpression &e) { return &e; }
ExpressionPtr Expression::MapVisitor::visit(StringExpression &e) { return &e; }
ExpressionPtr Expression::MapVisitor::visit(UnaryExpression &e) { return &e; }
ExpressionPtr Expression::MapVisitor::visit(VectorExpression &e) { return &e; }




