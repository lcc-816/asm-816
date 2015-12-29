#ifndef __defer_h__
#define __defer_h__


template<class FX>
class defer {
public:

	defer(FX &&fx) : __fx(std::forward<FX>(fx)), __engaged(true)
	{}

	defer(const defer &) = delete;

	defer(defer &&rhs) : __fx(std::move(rhs.__fx)), __engaged(rhs.__engaged)
	{
		rhs.__engaged = false;
	}

	~defer()
	{
		if (__engaged) __fx();
	}

	defer &operator=(const defer &) = delete;
	defer &operator=(defer &&rhs) {
		if (*this != rhs) {
			__fx = rhs.__fx;
			__engaged = rhs.__engaged;
			rhs.__engaged = false;
		}
		return *this;
	}

private:
	FX __fx;
	bool __engaged = false;
};

template<class FX>
defer<FX> make_defer(FX &&fx) {
	return defer<FX>(std::forward<FX>(fx));
}

#endif
