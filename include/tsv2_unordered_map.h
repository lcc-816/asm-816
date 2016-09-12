#ifndef __tsv2_unordered_map_h__
#define __tsv2_unordered_map_h__

#include <unordered_map>

// tsv2 stuff.

// unordered_map

template <class K, class T, class H, class P, class A, class Predicate>
void erase_if(std::unordered_map<K, T, H, P, A>& c, Predicate pred) {
	for (auto i = c.begin(), last = c.end(); i != last; ) {
	  if (pred(*i)) {
	    i = c.erase(i);
	  } else {
	    ++i;
	  }
	}
}
#endif
