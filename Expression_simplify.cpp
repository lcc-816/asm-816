#include "Expression_Internal.h"

using namespace Expression_Internal;


ExpressionPtr SimplifyVisitor::visit(UnaryExpression &e) {
	// e's children have been simplified at this point.

	return e.simplify_me();
}

ExpressionPtr SimplifyVisitor::visit(BinaryExpression &e) {
	// e's children have been simplified at this point.

	return e.simplify_me();
}


#pragma mark - simplify

ExpressionPtr Expression::simplify() {
    SimplifyVisitor v;
    return accept(v);
}


#if 0
ExpressionPtr Expression::simplify() {
    return this;
}

ExpressionPtr UnaryExpression::simplify() {

	std::transform(_children.begin(), _children.end(), _children.begin(), [](ExpressionPtr e){
		return e->simplify();
	});

	return simplify_me();
}

ExpressionPtr BinaryExpression::simplify() {

	std::transform(_children.begin(), _children.end(), _children.begin(), [](ExpressionPtr e){
		return e->simplify();
	});
    
	return simplify_me();
}


ExpressionPtr VectorExpression::simplify() {

	std::transform(_children.begin(), _children.end(), _children.begin(), [](ExpressionPtr e){
		return e->simplify();
	});
	
	return this;
}


#endif


ExpressionPtr UnaryExpression::simplify_me() {

	uint32_t value;

	if (_children[0]->is_integer(value)) {
		return Expression::Integer(unary_op(_op, value));
	}

	return this;
}

ExpressionPtr BinaryExpression::simplify_me() {

	uint32_t a,b;

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
		case '+':
		case '-':
			// int + rel
			{
				uint32_t pc;
				if (_children[1]->is_rel(pc)) {
					if (_op == '-') a = -a;
					return Expression::Rel(a + pc);
				}
			}
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

		case '+':
		case '-':
			// rel + int
			{
				uint32_t pc;
				if (_children[0]->is_rel(pc)) {
					if (_op == '-') b = -b;
					return Expression::Rel(pc + b);
				}
			}
		}
	}

	// relative - relative -> integer.
	// relative [any other op] relative is an error.
	if (_children[0]->is_rel(a) && _children[1]->is_rel(b)) {
		switch (_op) {
			case '-':
				return Expression::Integer(a - b);
			// logical ops?
		}
	}

	return this;
}

