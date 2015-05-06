
#include <string>
#include <unordered_map>

namespace {

	static std::unordered_multimap<size_t, const std::string *> ht;


}

const std::string *intern(const std::string &s) {

	std::hash<std::string> fx;

	size_t h = fx(s);

	auto iter = ht.find(h);
	while (iter != ht.end()) {
		if (iter->first != h) break;
		if (*iter->second == s) return iter->second;
	}

	std::string *sp = new std::string(s);
	ht.emplace(h, sp);
	return sp;
}



const std::string *intern(std::string &&s) {

	std::hash<std::string> fx;

	size_t h = fx(s);

	auto iter = ht.find(h);
	while (iter != ht.end()) {
		if (iter->first != h) break;
		if (*iter->second == s) return iter->second;
	}

	std::string *sp = new std::string(std::move(s));
	ht.emplace(h, sp);
	return sp;
}
