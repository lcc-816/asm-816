#ifndef __tsv2_deque_h__
#define __tsv2_deque_h__

#include <deque>

// tsv2 stuff.

// deque

template <class T, class A, class U>
void erase(std::deque<T, A>& c, const U& value) {
	c.erase(std::remove(c.begin(), c.end(), value), c.end());	
}

template <class T, class A, class Predicate>
void erase_if(std::deque<T, A>& c, Predicate pred) {
	c.erase(std::remove_if(c.begin(), c.end(), pred), c.end());
}

#endif
