#include "parser.h"
#include "lexer.h"
#include "grammar.h"
#include "Expression.h"
#include "cxx/mapped_file.h"

#include <cstdarg>

#include <iterator>
#include <system_error>

namespace {

	// symbol types
	enum {
		TypeUnknown,
		TypeEquate,
		TypeRecord,
		TypeMacro,
		TypeFunction,
		TypeData,
		TypeLabel
	};

};

bool parser::check_opcode(Token &minor) {
	Instruction instr(m65816, *minor.id);
	if (instr) {

		unsigned tk = tkOPCODE;
		if (instr.addressModes() == 1 << implied) tk = tkOPCODE_0;

		if (instr.hasAddressMode(block)) tk = tkOPCODE_2;
		if (instr.hasAddressMode(zp_relative)) tk = tkOPCODE_2;
		minor.type = tk;
		minor.value = instr;
		return true;
	}
	return false;
}

bool parser::check_macro(Token &minor) {
	return false;
}

bool parser::check_equate(Token &minor) {
	auto iter = _equates.find(minor.id);
	if (iter != _equates.end()) {
		ExpressionPtr e = iter->second;

		dp_register dp;
		if (e->is_register(dp)) {
			minor.type = tkDP_REGISTER;
			minor.value = dp;
		} else {
			minor.type = tkEXPRESSION;
			minor.value = e;
		}
		return true;
	}
	return false;
}


void parser::parse_token(Token &&token) {
	#if 0
	if (major == tkLABEL && token.id[0] == '@') {
		token.id = expand_at(token.id);
	}
	#endif

	if (token.type == tkEOL) _opcode = {};


	if (token.type == tkIDENTIFIER) {
		// check for opcodes.
		// check for macros.

		// macros are expanded in expand_macro.
		if (token.id->front() != '@')
			check_opcode(token) || check_equate(token);
	}
	if (_opcode.type == 0 && token.type != tkLABEL) _opcode = token;

	parse(token.type, std::move(token));
}

namespace {
	mapped_file find_include_path(filesystem::path &p, const std::vector<filesystem::path> &include_paths, std::error_code &ec) {

		ec.clear();
		if (p.is_absolute()) {
			return mapped_file(p, ec);
		}

		auto tmp = filesystem::absolute(p);
		auto f = mapped_file(tmp, ec);
		if (f) {
			p = std::move(tmp);
			return f;
		}

		for (auto tmp : include_paths) {
			tmp /= p;
			auto f = mapped_file(tmp, ec);
			if (f) {
				p = std::move(tmp);
				return f;
			}
		}
		ec = std::make_error_code(std::errc::no_such_file_or_directory);
		return mapped_file();
	}
}

void parser::expand_include(const Token &t, filesystem::path p) {
	std::error_code ec;

	if (p.empty()) {
		error(t, "Invalid include");
		return;
	}
	mapped_file f = find_include_path(p, _include_paths, ec);
	if (ec) {
		error(t, "error including %s: %s", p.c_str(), ec.message().c_str());
		return;	
	}

	lexer l(p);
	std::vector<Token> v;
	l.tokenize(f.begin(), f.end(), true, [&v](Token &&t){
		v.emplace_back(std::move(t));
	});
	for (auto iter = v.rbegin(); iter != v.rend(); ++iter) {
		_queue.emplace_front(std::move(*iter));
	}
	#if 0
	_queue.insert(_queue.front(), 
		std::make_move_iterator(v.begin()), 
		std::make_move_iterator(v.end())
	);
	#endif
}

bool parser::install_equate(const Token &t, ExpressionPtr &e) {

	if (!t.id) return false; // ?
	const std::string &name = t.string_value();
	// top level vs not top level?

	if (_seg_type == none && t.int_value() == 1) {
		error(t, "Local label may only be used inside a segment");
		return false;
	}

	if (t.int_value() == 2) {
		error(t, "Macro label may only be used inside a macro");
		return false;
	}

	identifier id = expand_at(t.id);

	{
		auto iter = _equates.find(id);
		if (iter != _equates.end()) {
			warn(t, "Equate %s previously defined", name.c_str());
		}
	}

	if (_labels.count(id) > 0) {
		error(t, "Duplicate label");
		return false;
	}

	// how to handle *...

	auto rv = _equates.emplace(t.id, e->simplify());
	return rv.second; // true if inserted, false if duplicate.
}


namespace {
#if 0
	template<class T>
	int find(const std::vector<T> &v, const T& value) {
		for (int i = 0; i < v.size; ++i) if (v[i] == value) return i;
		return -1;
	}


	std::vector<int> find_commas(const std::vector<Token> &tokens) {
		int i;
		int bracket = 0;
		int paren = 0;

		std::vector<int> commas;

		for (i = 0; i < tokens.size(); ++i) {
			const auto &t = tokens[i];
			switch (t) {
				case tkLPAREN: paren++; break;
				case tkRPAREN: paren--; break;
				case tkLBRACKET: bracket++; break;
				case tkRBRACKET: bracket--; break;
				case tkCOMMA:
					if (!paren && !bracket) { commas.emplace_back(i); }
					break;
			}
		}
		return commas;
	}
#endif


	typedef std::vector< std::pair<identifier, std::vector<Token> > > arg_vector;
	typedef std::vector<Token> token_vector;

	arg_vector
	parse_parameters(const std::vector<identifier> &formal, bool variadic, std::vector<Token> &&tokens) {

		static identifier dotdotdot = nullptr;

		if (!dotdotdot) dotdotdot = intern("&...");
		arg_vector rv;
		rv.reserve(formal.size() + 1);

		if (formal.empty()) {
			rv.emplace_back(std::make_pair(dotdotdot, std::move(tokens)));
			return rv;		
		}

		unsigned j = 0;
		int bracket = 0;
		int paren = 0;
		auto begin = tokens.begin();

		for (auto iter = tokens.begin(); iter != tokens.end(); ++iter) {
			int _break = 0;
			const auto &t = *iter;
			switch (t.type) {
				case tkLPAREN: paren++; break;
				case tkRPAREN: paren--; break;
				case tkLBRACKET: bracket++; break;
				case tkRBRACKET: bracket--; break;
				case tkCOMMA:
					if (!paren && !bracket) {

						// check j length...
						if (j >= tokens.size()) {
							throw std::runtime_error("macro arity error");
						}
						identifier name = formal[j++];
						if (j == tokens.size() && variadic) {
							rv.emplace_back( std::make_pair(name, token_vector(begin, tokens.end())));
							_break = 1;
						} else {
							rv.emplace_back( std::make_pair(name, token_vector(begin, iter)));
							begin = iter + 1;
						}
					}
					break;
			}
			if (_break) break;
		}

		// final parm.
		if (paren) throw std::runtime_error("unbalanced ()");
		if (bracket) throw std::runtime_error("unbalanced {}");
		if (j >= tokens.size()) throw std::runtime_error("macro arity error");
		rv.emplace_back( std::make_pair(formal[j++], token_vector(begin, tokens.end())));

		// empty parameters
		while (j < formal.size()) {
			rv.emplace_back ( std::make_pair(formal[j++], token_vector() ));
		}

		rv.emplace_back(std::make_pair(dotdotdot, std::move(tokens)));
		return rv;
	}

	arg_vector::const_iterator find(const arg_vector &tv, identifier name) {
		return std::find_if(tv.begin(), tv.end(), [name](const auto &e){ return e.first == name; });
	} 
} // namespace

void parser::expand_macro(const Token &t, std::vector<Token> &&arguments) {

	// todo -- capture label as part of macro.
	// todo -- expand @identifiers in arguments before entering macro scope.

	auto name = t.string_value();
	auto iter = _macros.find(t.id);
	if (iter == _macros.end()) {
		error(t, "Undefined macro or opcode: %s", name.c_str());
		return;
	}

	const auto &m = iter->second;
	// check arity?


	auto env = parse_parameters(m.params, m.variadic, std::move(arguments));

	// no longer 1-1 association, now multi-tokens...
	std::vector<Token> v;

	for (const auto &t : m.body) {
		if (t.type == tkMPARAM) {
			auto iter = find(env, t.id);
			if (iter == env.end()) {
				error(t, "Unbound macro parameter: %s", t.string_value().c_str());
				continue;
			}
			const auto &tt = iter->second;
			v.insert(v.end(), tt.begin(), tt.end());
			continue;
		}
		// special logic for tkLABEL w/ int_value == 1 ....
		if (t.type == tkLABEL && t.int_value() == 2) {
			// label is actually macro parameter. expand and convert first token to a label.
			auto iter = find(env, t.id);
			if (iter == env.end()) {
				error(t, "Unbound macro parameter: %s", t.string_value().c_str());
				continue;
			}
			auto tt = iter->second; // copy since it may be modified.
			if (!tt.empty()) {
				auto &t = tt.front();
				if (t.type == tkIDENTIFIER) { // or fallsback to identifier.. {
					t.type = tkLABEL;
					t.value = 0;
				}
			}
			v.insert(v.end(), tt.begin(), tt.end());
			continue;
		}
		v.emplace_back(t);
	}

	for (auto iter = v.rbegin(); iter != v.rend(); ++iter) {
		_queue.emplace_front(std::move(*iter));
	}

}


bool parser::install_macro(const Token &t, const std::vector<Token> &formal) {

	// 1. check
	bool ok = true;

	_tmp_macro.name = t.id;
	_tmp_macro.file = t.file;
	_tmp_macro.line = t.line;

	auto iter = _macros.find(t.id);
	if (iter == _macros.end()) {

		std::unordered_set<identifier> set;
		// check for variadic.
		if (!formal.empty()) {
		 	if (formal.back().int_value() == 1)
				_tmp_macro.variadic = true;

			for (unsigned i = 0; i < formal.size() - 1; ++i) {
				if (formal[i].int_value() != 0) {
					const auto &t = formal[i];
					error(t, "%s: Only final parameter may be variadic.", t.id->c_str());
					ok = false;
				}
			}

			// check for reserved and duplicate names.
			// todo - allow &...
			for (const auto &t : formal) {
				if (t.int_value() > 1) {
					error(t, "%s: Reserved parameter name.", t.id->c_str());
					ok = false;
				}
				if (set.count(t.id)) {
					error(t, "%s: Duplicate parameter name.", t.id->c_str());
					ok = false;
				}
			}
		}



		// check for unbound parameters
		// 
		for (const auto &t : _tmp_macro.body) {
			if (t.type == tkMPARAM && !set.count(t.id)) {
				error(t, "%s: Unbound parameter.", t.id->c_str());
				ok = false;
			}
			if (t.type == tkLABEL && t.int_value() == 2 && !set.count(t.id)) {
				error(t, "%s: Unbound parameter.", t.id->c_str());
				ok = false;		
			}
		}



		std::transform(formal.begin(), formal.end(),
			std::back_inserter(_tmp_macro.params),
			[](const auto &t){ return t.id; });

		if (!ok) _tmp_macro.body.clear();
		_macros.emplace(t.id, std::move(_tmp_macro) );

	} else {
		error(t, "Macro %s previously defined", t.id->c_str());
		ok = false;
	}

	_tmp_macro = {};
	return ok;
}



bool parser::install_record(const Token &t) {
	// check for @ or &...
	auto iter = _records.find(t.id);
	if (iter != _records.end()) {
		error(t, "Duplicate record %s", t.string_value().c_str());
		_tmp_record = {};
		return false;
	}

	_tmp_record.name = t.id;
	_tmp_record.file = t.file;
	_tmp_record.line = t.line;
	_records.emplace(t.id, std::move(_tmp_record));
	_tmp_record = {};
	return true;
}

identifier parser::install_record_label(const Token &t) {

	// check for duplicates..
	identifier id = t.id;
	auto &fields = _tmp_record.fields;

	if (std::any_of(fields.begin(), fields.end(), [id](const field &f){ return f.name == id; })) {

		error(t, "Duplicate record label %s", id->c_str());
		return nullptr;
	}

	// check for @ or &....

	if (t.int_value() == 1) {
		error(t, "Invalid macro parameter %s", id->c_str());
		return nullptr;
	}

	if (t.int_value() == 2) {
		error(t, "Invalid local parameter %s", id->c_str());
		return nullptr;
	}

	fields.emplace_back( field{id, _tmp_record.size, nullptr});
	return id;
}

bool parser::install_record_ds(const Token &t, identifier label, int modifier, ExpressionPtr e){

	// label has already been installed (and is presumed to be the last entry);

	identifier r;
	if (e->is_identifier(r)) {
		// ds recordname
		auto iter = _records.find(r);
		if (iter == _records.end()) {
			error(t, "Unknown record %s", r->c_str());
			return false;
		}

		if (modifier) {
			error(t, "invalid DS modifier for record.");
		}

		if (label)
			_tmp_record.fields.back().record = r;

		_tmp_record.size += iter->second.size;
		return true;
	}

	e = e->simplify();
	uint32_t i;
	if (e->is_integer(i)) {
		// ds 1
		// ds.b 1
		// ds.w 1
		// ds.l 1
		if (!modifier) modifier = 1;
		_tmp_record.size += modifier * i;
		return true;
	}

	error(t, "Expression too complex.");
	return false;
}


int parser::field_offset(const record &rec, std::vector<Token> &tokens) {

	int offset = 0;
	const record *r = &rec;
	const field *f = nullptr;
	identifier prev = nullptr;

	for (const auto &t : tokens) {

		if (!r) {
			error(t, "Invalid record %s", prev->c_str());
			return 0;
		}

		prev = t.id;
		auto iter = std::find_if(r->fields.begin(), r->fields.end(),
			[&t](const field &f){ return f.name == t.id; });

		if (iter == r->fields.end()) {
			error(t, "Invalid field %s", t.id->c_str());
			return 0;
		}
		f = &(*iter);
		offset += f->offset;
		r = nullptr;
		if (f->record) {
			auto iter = _records.find(f->record);
			if (iter != _records.end()) r = &(iter->second);
		}

	}
	return offset;
}




void parser::resolve_records() {

	/*
	 * to handle forward record references, (eg)
	 * 
	 *     lda dcb.refNum
	 *     ...
	 * dcb ds CloseRecGS
	 *
	 * we defer record field resolution until after the segment is finished.
	 * 
	 */

	std::unordered_map<identifier, ExpressionPtr> map;

	for (auto &x : _pending_records) {
		auto id = x.first;
		auto tokens = std::move(x.second);

		auto t = std::move(tokens.back());
		tokens.pop_back();

		// always a label.  :record.field handled by parser.

		identifier rid = nullptr;
		auto iter = _types.find(t.id);
		if (iter == _types.end()) {
			error(t, "Undeclared variable %s", t.id->c_str());
			continue;	
		}

		auto rIter = _records.find(rid);
		if (rIter == _records.end()) {
			error(t, "Invalid record %s", rid->c_str());
			continue;
		}

#if 0
		// check if this is a record or a label.  or both.

		auto tIter = _types.find(t.id);
		auto rIter = _records.find(t.id);

		if (tIter && rIter != _records.end()) {
			error(t, "Ambiguous record: %s", t.id->c_str());
			continue;
		}

		if (tIter) {
			rIter = _records.find(tIter);
			e = Expression::Identifier(t.id);
		}

		if (rIter == _records.end()) {
			error(t, "Invalid record: %s", t.id->c_str());
			continue;
		}
#endif
		int offset = field_offset(rIter->second, tokens);

		ExpressionPtr e = Expression::Identifier(t.id);
		if (offset) e = Expression::Binary('+', e, Expression::Integer(offset));
		map.emplace(id, e);
	}


	if (map.empty()) {
		_pending_records.clear();
		return;

	}
	std::function<ExpressionPtr(identifier)> fx([&](identifier id){

		auto iter = map.find(id);
		if (iter == map.end()) return ExpressionPtr();
		return iter->second;
	});

	for(auto &line : _lines) {
		for (auto &e : line->operands) {
			if (e) {
				e = e->replace_identifier(fx);
			}
		}
	}


	_pending_records.clear();
}


void parser::verify_identifiers() {
	/* if -W, verify all identifiers are known */


	for(auto &line : _lines) {
		for (const auto &e : line->operands) {
			if (e) {
				auto identifiers = e->identifiers();
				for (auto id : identifiers) {
					if (_import_set.find(id) != _import_set.end()) continue;
					if (_entry_set.find(id) != _entry_set.end()) continue;
					if (_labels.find(id) != _labels.end()) continue;

					fprintf(stderr, "# Udnefined ID %s", id->c_str());
					_error++;
				}
			}
		}
	}	


}



void parser::add_ds(Token &t, identifier label, int modifier, ExpressionPtr e) {
	identifier id;
	if (e->is_identifier(id)) {
		// record?
		auto iter = _records.find(id);
		if (iter == _records.end()) {
			error(t, "Invalid record %s", id->c_str());
			return;
		}

		if (modifier) {
			error(t, "invalid DS modifier for record.");
		}

		// type the label.
		if (label) _types.emplace(label, id);

		add_line(BasicLine::Make(DS, Expression::Integer(iter->second.size)));
		return;
	}
	if (modifier > 1) e = Expression::Binary('*', Expression::Integer(modifier), e);
	e = e->simplify();
	add_line(BasicLine::Make(DS, std::move(e)));
}

void parser::drain_queue() {
	while (!_queue.empty()) {
		auto t = std::move(_queue.front());
		_queue.pop_front();
		parse_token(std::move(t));
	}
}


void parser::begin_segment(identifier name, SegmentType type) {

	if (_segment) {
		error("Error: Segment still active");
		end_segment();
	}
	if (name == nullptr && type == data) {
		_segment = _data_segment.get();
	} else {
		_segments.emplace_back(std::make_unique<Segment>());
		_segment = _segments.back().get();
		_segment->name = name;
	}

	_seg_type = type;
	_equates.push_scope();
	_types.push_scope();
	_import_set.push_scope();
	_entry_set.push_scope();

	_labels.clear();
	_labels.insert(name);
	_current_label = name;
}

void parser::end_segment() {
	if (!_segment) {
		error("Error: No active segment");
	} else {

		resolve_records();
		verify_identifiers();
		_segment->lines = std::move(_lines);
		_equates.pop_scope();
		_types.pop_scope();
		_import_set.pop_scope();
		_entry_set.pop_scope();
	}


	_segment = nullptr;
	_lines.clear();
	_seg_type = none;
	_current_label = nullptr;
}


identifier parser::add_label(const Token &t, bool hidden) {
	if (!t.id) return nullptr;

	// local labels only valid in segment/record/macro
	if (_seg_type == none && t.int_value() == 1) {
		error(t, "Local label may only be used inside a segment");
		return nullptr;
	}

	if (t.int_value() == 2) {
		error(t, "Macro label may only be used inside a macro");
		return nullptr;
	}

	// expand '@' ...
	identifier id = expand_at(t.id);
	// check for duplicates...
	if (_equates.count(id)) {
		error(t, "Label conflict with equate");
		return nullptr;
	}

	if (_labels.count(id) > 0) {
		error(t, "Duplicate label");
		return nullptr;
	}

	if (_records.count(id)) {
		warn(t, "Label conflict with record");
		// but insert it anyhow to prevent later errors.
	}

	_labels.insert(id);	
	_lines.emplace_back(BasicLine::Make(id));
	if (!hidden && t.int_value() == 0) _current_label = id;
	return id;
}

// only used for symbol-generated labels.
identifier parser::add_label(identifier label, bool hidden) {
	if (!label) return nullptr;


	// check for errors...

	_lines.emplace_back(BasicLine::Make(label));


	if (!hidden && label->find('@') == label->npos)
		_current_label = label;

	return label;
}

void parser::add_line(BasicLinePtr &&line) {
	if (!line) return;
	_lines.emplace_back(std::move(line));
}

void parser::add_lines(std::vector<BasicLinePtr> &&lines) {
	for (auto &x : lines)
		if (x) _lines.emplace_back(std::move(x));
}

std::unique_ptr<Module> parser::module() {

	// ???
	//if (_error) return nullptr;

	auto m = std::make_unique<Module>();

	std::transform(_import_set.begin(), _import_set.end(), std::back_inserter(m->imports),
		[](const auto &e){ return e.first; });

	//m->imports.assign(_import_set.begin(), _import_set.end());
	m->exports.assign(_export_set.begin(), _export_set.end());


	if (_data_segment)
		_segments.emplace_back(std::move(_data_segment));

	// set exports.
	for (auto &seg : _segments) {

		identifier label = seg->name;
		if (label && _export_set.count(label))
			seg->global = true;

		for (auto &line : seg->lines) {
			identifier label = line->label;
			if (label && _export_set.count(label))
				line->global = true;
		}

	}

	m->segments = std::move(_segments);
	_segments.clear();

	return m;
}

identifier parser::expand_at(identifier id) {
	if (!id || id->front() != '@') return id;
	if (_current_macro) {
		return intern(*_current_macro + *id);
	}
	if (_current_label)
		return intern(*_current_label + *id);
	return id;
}


identifier parser::expand_at(const Token &t) {

	identifier id = t.id;
	if (_seg_type == none && id->front() == '@') {
		error(t, "Local label may only be used inside a segment");
		return nullptr;
	}

	if (id->front() == '&') {
		error(t, "Macro label may only be used inside a macro");
		return nullptr;
	}

	return expand_at(t.id);
}



void parser::parse_failure()
{
	if (!_error) _error = 1;
}

identifier parser::gen_sym() {
	std::string s(":");
	s += std::to_string(++_gen_sym);
	return intern(s);
}

void parser::error(const Token &where, const char *format, ...) {
	va_list ap;
	va_start(ap, format);
	verror(where, format, ap);
	va_end(ap);
}

void parser::error(const char *format, ...) {
	va_list ap;
	va_start(ap, format);
	verror(_opcode, format, ap);
	va_end(ap);
}

void parser::verror(const Token &where, const char *format, va_list ap) {

	if (where.file && where.line) {
		fprintf(stderr, "#%s:%d Error: ", where.file->c_str(), where.line);
	} else {
		fprintf(stderr, "# Error: ");
	}
	vfprintf(stderr, format, ap);
	fputc('\n', stderr);
	_error++;
}


void parser::warn(const Token &where, const char *format, ...) {
	va_list ap;
	va_start(ap, format);
	vwarn(where, format, ap);
	va_end(ap);
}

void parser::warn(const char *format, ...) {
	va_list ap;
	va_start(ap, format);
	vwarn(_opcode, format, ap);
	va_end(ap);
}

void parser::vwarn(const Token &where, const char *format, va_list ap) {

	if (where.file && where.line) {
		fprintf(stderr, "#%s:%d Warning: ", where.file->c_str(), where.line);
	} else {
		fprintf(stderr, "# Warning: ");
	}
	vfprintf(stderr, format, ap);
	fputc('\n', stderr);
	_warn++;
}

