#include "Expression_Internal.h"


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
CREATE_VISITOR(RelExpression);
CREATE_VISITOR(StringExpression);



void VectorExpression::accept(ConstVisitor &v) const {
	v.visit(*this);
	for (auto e : _children) e->accept(v);
}

void UnaryExpression::accept(ConstVisitor &v) const {
	v.visit(*this);
	for (auto e : _children) e->accept(v);
}

void BinaryExpression::accept(ConstVisitor &v) const {
	v.visit(*this);
	for (auto e : _children) e->accept(v);
}

#undef CREATE_VISITOR
#define CREATE_VISITOR(klass) void klass::accept(ConstVisitor &v) const { v.visit(*this); }
CREATE_VISITOR(IdentifierExpression);
CREATE_VISITOR(IntegerExpression);
CREATE_VISITOR(PCExpression);
CREATE_VISITOR(RegisterExpression);
CREATE_VISITOR(RelExpression);
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
CREATE_VISITOR(RelExpression);
CREATE_VISITOR(StringExpression);


#pragma mark - visitors


Expression::Visitor::~Visitor() {}
void Expression::Visitor::visit(BinaryExpression &) {}
void Expression::Visitor::visit(IdentifierExpression &) {}
void Expression::Visitor::visit(IntegerExpression &) {}
void Expression::Visitor::visit(PCExpression &) {}
void Expression::Visitor::visit(RegisterExpression &) {}
void Expression::Visitor::visit(RelExpression &) {}
void Expression::Visitor::visit(StringExpression &) {}
void Expression::Visitor::visit(UnaryExpression &) {}
void Expression::Visitor::visit(VectorExpression &) {}

Expression::ConstVisitor::~ConstVisitor() {}
void Expression::ConstVisitor::visit(const BinaryExpression &) {}
void Expression::ConstVisitor::visit(const IdentifierExpression &) {}
void Expression::ConstVisitor::visit(const IntegerExpression &) {}
void Expression::ConstVisitor::visit(const PCExpression &) {}
void Expression::ConstVisitor::visit(const RegisterExpression &) {}
void Expression::ConstVisitor::visit(const RelExpression &) {}
void Expression::ConstVisitor::visit(const StringExpression &) {}
void Expression::ConstVisitor::visit(const UnaryExpression &) {}
void Expression::ConstVisitor::visit(const VectorExpression &) {}


Expression::MapVisitor::~MapVisitor() {}
// these should all be const, but that screws up the return type.
ExpressionPtr Expression::MapVisitor::visit(BinaryExpression &e) { return &e; }
ExpressionPtr Expression::MapVisitor::visit(IdentifierExpression &e) { return &e; }
ExpressionPtr Expression::MapVisitor::visit(IntegerExpression &e) { return &e; }
ExpressionPtr Expression::MapVisitor::visit(PCExpression &e) { return &e; }
ExpressionPtr Expression::MapVisitor::visit(RegisterExpression &e) { return &e; }
ExpressionPtr Expression::MapVisitor::visit(RelExpression &e) { return &e; }
ExpressionPtr Expression::MapVisitor::visit(StringExpression &e) { return &e; }
ExpressionPtr Expression::MapVisitor::visit(UnaryExpression &e) { return &e; }
ExpressionPtr Expression::MapVisitor::visit(VectorExpression &e) { return &e; }


