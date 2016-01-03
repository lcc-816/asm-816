#ifndef __tsv2_map_h__
#define __tsv2_map_h__

#include <map>

// tsv2 stuff.

// map

template <class K, class T, class C, class A, class Predicate>
void erase_if(std::map<K, T, C, A>& c, Predicate pred) {
	for (auto i = c.begin(), last = c.end(); i != last; ) {
	  if (pred(*i)) {
	    i = c.erase(i);
	  } else {
	    ++i;
	  }
	}
}

#endif
