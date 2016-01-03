#ifndef __tsv2_vector_h__
#define __tsv2_vector_h__

#include <vector>

// tsv2 stuff.

// vector

template <class T, class A, class U>
void erase(std::vector<T, A>& c, const U& value) {
	c.erase(std::remove(c.begin(), c.end(), value), c.end());	
}

template <class T, class A, class Predicate>
void erase_if(std::vector<T, A>& c, Predicate pred) {
	c.erase(std::remove_if(c.begin(), c.end(), pred), c.end());
}

#endif
