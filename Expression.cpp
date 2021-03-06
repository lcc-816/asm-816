#include <deque>
#include <string>
#include <algorithm>
#include <memory>

#include <cstdio>

#include "Expression_Internal.h"


namespace Expression_Internal {

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


	std::string int_to_string(uint32_t x) {
		char buffer[10];
		if (x <= 0xff) snprintf(buffer, sizeof(buffer), "$%02x", x);
		else if (x <= 0xffff) snprintf(buffer, sizeof(buffer), "$%04x", x);
		else if (x <= 0xffffff) snprintf(buffer, sizeof(buffer), "$%06x", x);
		else snprintf(buffer, sizeof(buffer), "$%08x", x);

		return std::string(buffer);
	}

}
using namespace Expression_Internal;



#pragma mark - destructors

Expression::~Expression() {
}

VectorExpression::~VectorExpression() {
}




#pragma mark - constructors
ExpressionPtr Expression::PC() {
	static ExpressionPtr pc = std::make_shared<PCExpression>();
	return pc;
}

ExpressionPtr Expression::Rel(uint32_t offset) {
	ExpressionPtr e = std::make_shared<RelExpression>(offset);
	return e;
}

ExpressionPtr Expression::Integer(uint32_t value) {
	ExpressionPtr e = std::make_shared<IntegerExpression>(value);
	return e;
}

ExpressionPtr Expression::Register(dp_register value) {
	ExpressionPtr e = std::make_shared<RegisterExpression>(value);
	return e;
}

ExpressionPtr Expression::Identifier(const std::string *value) {
	ExpressionPtr e = std::make_shared<IdentifierExpression>(value);
	return e;
}

ExpressionPtr Expression::WeakIdentifier(const std::string *value) {
	ExpressionPtr e = std::make_shared<IdentifierExpression>(value, true);
	return e;
}


ExpressionPtr Expression::String(const std::string *value) {
	ExpressionPtr e = std::make_shared<StringExpression>(value);
	return e;
}


ExpressionPtr Expression::Unary(unsigned op, ExpressionPtr child) {
	ExpressionPtr e = std::make_shared<UnaryExpression>(op, child);
	return e;
}

ExpressionPtr Expression::Binary(unsigned op, ExpressionPtr a, ExpressionPtr b) {
	ExpressionPtr e = std::make_shared<BinaryExpression>(op, a, b);
	return e;
}

VectorExpressionPtr Expression::Vector() {
	VectorExpressionPtr e = std::make_shared<VectorExpression>();
	return e;
}

VectorExpressionPtr Expression::Vector(const std::vector<ExpressionPtr> &children) {
	VectorExpressionPtr e = std::make_shared<VectorExpression>(children);
	return e;
}

VectorExpressionPtr Expression::Vector(std::vector<ExpressionPtr> &&children) {
	VectorExpressionPtr e =  std::make_shared<VectorExpression>(std::move(children));
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

bool Expression::is_rel(uint32_t &) const {
	return false;
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

bool RelExpression::is_rel(uint32_t &rv) const {
	rv = _offset;
	return true;
}




#pragma mark - to_string

void PCExpression::to_string(std::string &rv) const {
	rv.push_back('*');
}

void RelExpression::to_string(std::string &rv) const {
	rv.append("__start+");
	rv.append(int_to_string(_offset));
}

void IntegerExpression::to_string(std::string &rv) const {
	rv.append(int_to_string(_value));
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




#pragma mark - rename
namespace {

	class RenameIdentifierVisitor : public Expression::MapVisitor {
	public:
		RenameIdentifierVisitor(identifier from, identifier to) : _from(from), _to(to)
		{}

		virtual ExpressionPtr visit(const IdentifierExpression &e) override final {
			identifier id;
			if (e.is_identifier(id))
				if (id == _from) return Expression::Identifier(_to);

			return to_expression_ptr(e);
		}

	private:
		identifier _from;
		identifier _to;

	};



	class RenameRegisterVisitor : public Expression::MapVisitor {
	public:
		RenameRegisterVisitor(dp_register from, dp_register to) : _from(from), _to(to)
		{}

		virtual ExpressionPtr visit(const RegisterExpression &e) override final {
			dp_register r;
			if (e.is_register(r))
				if (r == _from) return Expression::Register(_to);

			return to_expression_ptr(e);
		}

	private:
		dp_register _from;
		dp_register _to;

	};


}

ExpressionPtr Expression::rename(identifier from, identifier to) {

	RenameIdentifierVisitor v(from, to);
	return accept(v);
}

ExpressionPtr Expression::rename(dp_register from, dp_register to) {

	RenameRegisterVisitor v(from, to);
	return accept(v);
}



#pragma mark - replace

	class ReplaceIdentifierVisitor : public Expression::MapVisitor {
	public:
		ReplaceIdentifierVisitor(std::function<ExpressionPtr(identifier)> &fx) : _fx(fx)
		{}

		virtual ExpressionPtr visit(const IdentifierExpression &e) override final {

			identifier id;
			if (e.is_identifier(id)) {
				auto e = _fx(id);
				if (e) return e;
			}

			return to_expression_ptr(e);
		}

	private:
		std::function<ExpressionPtr(identifier)> &_fx;
	};


ExpressionPtr Expression::replace_identifier(std::function<ExpressionPtr(identifier)> &fx) {

	ReplaceIdentifierVisitor v(fx);
	return accept(v);
}


#pragma mark - identifiers

namespace {
	class IdentifierVisitor : public Expression::ConstVisitor {
	public:
		std::vector<identifier> identifiers;
		virtual void visit(const IdentifierExpression &e) override final {
			identifier id;
			if (e.is_identifier(id)) identifiers.push_back(id);
		}
	};
}

std::vector<identifier> Expression::identifiers() const {
	IdentifierVisitor v;

	accept(v);
	return v.identifiers;
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
	// not actually useful.  should convert to a RelExpression
	// first.
	rv.push_back(0x80);
}

void RelExpression::to_omf(std::vector<uint8_t> &rv) const {
		rv.push_back(0x87);

		rv.push_back(_offset >> 0 );
		rv.push_back(_offset >> 8 );
		rv.push_back(_offset >> 16);
		rv.push_back(_offset >> 24);	
}

void IdentifierExpression::to_omf(std::vector<uint8_t> &rv) const {

	rv.push_back(_weak ? 0x82 : 0x83);
	rv.push_back(_value->length());

	for (auto c : *_value) rv.push_back(c);

}


#pragma mark - operator ==

bool Expression::operator==(const Expression &) const {
	return false;
}

bool IntegerExpression::operator==(const Expression &rhs) const {
	if (&rhs == this) return true;
	if (rhs.is_integer()) return *this == static_cast<const IntegerExpression &>(rhs);
	return false;
}

bool IntegerExpression::operator==(const IntegerExpression &rhs) const {
	return _value == rhs._value;
}


bool RegisterExpression::operator==(const Expression &rhs) const {
	if (&rhs == this) return true;

	if (rhs.is_register()) return *this == static_cast<const RegisterExpression &>(rhs);
	return false;
}

bool RegisterExpression::operator==(const RegisterExpression &rhs) const {
	return _value == rhs._value;
}

bool IdentifierExpression::operator==(const Expression &rhs) const {
	if (&rhs == this) return true;

	if (rhs.is_identifier()) return *this == static_cast<const IdentifierExpression &>(rhs);
	return false;
}

bool IdentifierExpression::operator==(const IdentifierExpression &rhs) const {
	return _value == rhs._value;
}

bool StringExpression::operator==(const Expression &rhs) const {
	if (&rhs == this) return true;

	if (rhs.is_string()) return *this == static_cast<const StringExpression &>(rhs);
	return false;
}

bool StringExpression::operator==(const StringExpression &rhs) const {
	return _value == rhs._value;
}

#if 0
bool RelExpression::operator==(const Expression &rhs) const {
	if (rhs.is_integer()) return *this == static_cast<const RelExpression &>(rhs);
	return false;
}

bool RelExpression::operator==(const RelExpression &rhs) const {
	return _offset == rhs._offset;
}
#endif

bool UnaryExpression::operator==(const Expression &rhs) const {
	if (&rhs == this) return true;

	if (rhs.is_unary()) return *this == static_cast<const UnaryExpression &>(rhs);
	return false;
}

bool UnaryExpression::operator==(const UnaryExpression &rhs) const {
	if (&rhs == this) return true;

	return _op == rhs._op && *rhs._children[0] == *rhs._children[0];
}


bool BinaryExpression::operator==(const Expression &rhs) const {
	if (&rhs == this) return true;

	if (rhs.is_binary()) return *this == static_cast<const BinaryExpression &>(rhs);
	return false;
}

bool BinaryExpression::operator==(const BinaryExpression &rhs) const {
	if (&rhs == this) return true;
	return _op == rhs._op && *_children[0] == *rhs._children[0] && *_children[1] == *rhs._children[1];
}


bool VectorExpression::operator==(const VectorExpression &rhs) const {
	if (&rhs == this) return true;

	return std::equal(_children.begin(), _children.end(), 
		rhs._children.begin(), rhs._children.end(),
		[](ExpressionPtr a, ExpressionPtr b) { return *a == *b; });
}

bool VectorExpression::operator==(const Expression &rhs) const {
	if (rhs.is_vector()) return *this == static_cast<const VectorExpression &>(rhs);
	return false;
}

namespace {



	class SetPCVisitor : public SimplifyVisitor {
	public:
		SetPCVisitor(uint32_t pc) : _pc(pc)
		{}

		virtual ExpressionPtr visit(const PCExpression &) override final {
			return Expression::Rel(_pc);
		}
		using SimplifyVisitor::visit;

	private:
		uint32_t _pc;
	};

	class MakeRelativeVisitor : public SetPCVisitor {

	public:
		MakeRelativeVisitor(uint32_t pc, const identifier_map &env):
			SetPCVisitor(pc), _env(env)
		{}

		virtual ExpressionPtr visit(const IdentifierExpression &e) override final {
			identifier id;
			if (e.is_identifier(id)) {
				auto iter = _env.find(id);
				if (iter != _env.end())
					return Expression::Rel(iter->second);
			}
			return to_expression_ptr(e);
		}
		using SetPCVisitor::visit;
	private:
		const identifier_map &_env;
	};


}

ExpressionPtr Expression::make_relative(uint32_t pc, const identifier_map &env) {
	MakeRelativeVisitor v(pc, env);
	return accept(v);
}

ExpressionPtr Expression::make_relative(uint32_t pc) {

	SetPCVisitor v(pc);

	return accept(v);
}


namespace {

	class RRVisitor : public Expression::MapVisitor {

	public:
		RRVisitor(const Expression::register_info &ri) : _ri(ri)
		{}

		virtual ExpressionPtr visit(const RegisterExpression &e) override final {

			dp_register r;
			if (e.is_register(r)) {
				switch (r.type) {
					case 'r':
					case 't':
						{
							auto iter = _ri.trmap.find(r);
							if (iter == _ri.trmap.end())
								throw std::runtime_error("Unable to assign register");

							return Expression::Integer(iter->second);
						}
					case 'v':
						return Expression::Integer(r.number + _ri.vbase);
					case 'p':
						return Expression::Integer(r.number + _ri.pbase);
				}
			}
			return to_expression_ptr(e);
		}

	private:
		const Expression::register_info &_ri;
	};

}

ExpressionPtr Expression::assign_registers(const register_info &ri) {

	RRVisitor v(ri);

	return accept(v);
}


