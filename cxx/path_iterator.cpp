
#include "filesystem.h"
#include <iterator>

class path_iterator {

	path operator*() {
		
	}

private:

	friend class path;

	path_iterator(path &p path) : _path(path)
	{}

	path &_path;


};
