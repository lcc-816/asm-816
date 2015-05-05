#include "bitvector.h"
#include <vector>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <numeric>


// std::numeric_limits<size_t>::max() instead of -1;

namespace {

	const size_t NegativeOne = std::numeric_limits<size_t>::max();


}

template<class T> 
struct logical_true : public std::unary_function<T, bool> {

	bool operator ()(const T& arg) const 
	{ return (bool)arg; }
};


template<class T> 
struct bit_negate : public std::unary_function<T, T>{

	T operator ()(const T& arg) const 
	{ return ~arg; }
};


#if 0
void bitvector::shrink_to_fit() {

	// remove any trailing 0s...?

	while (!_bits.empty()) {
		if (_bits.last()) break;
		_bits.pop_back();
	}

	_bits.shrink_to_fit();
}
#endif

std::tuple<unsigned, unsigned> bitvector::index_mask(size_t pos, const char *func) const {

	unsigned index = pos / sizeof(size_t);
	unsigned mask = 1 << (pos % sizeof(size_t));

	if (index >= _bits.size()) throw std::out_of_range(func);

	return std::make_tuple(index, mask);
}

std::tuple<unsigned, unsigned> bitvector::index_mask(size_t pos) const noexcept {

	unsigned index = pos / sizeof(size_t);
	unsigned mask = 1 << (pos % sizeof(size_t));

	return std::make_tuple(index, mask);
}

#pragma mark - bit ops

bitvector bitvector::operator~() const {
	bitvector tmp(*this);
	return tmp.flip();
}


bitvector& bitvector::operator&=( const bitvector& other ) {
	if (_bits.size() < other._bits.size())
		_bits.resize(other._bits.size(), 0);

	auto iter = _bits.begin();
	for (auto x : other._bits)
		*iter++ &= x;

	return *this;
}


bitvector& bitvector::operator|=( const bitvector& other ) {
	if (_bits.size() < other._bits.size())
		_bits.resize(other._bits.size(), 0);

	auto iter = _bits.begin();
	for (auto x : other._bits)
		*iter++ |= x;

	return *this;
}

bitvector& bitvector::operator^=( const bitvector& other ) {
	if (_bits.size() < other._bits.size())
		_bits.resize(other._bits.size(), 0);

	auto iter = _bits.begin();
	for (auto x : other._bits)
		*iter++ ^= x;

	return *this;
}

bool bitvector::test( size_t pos ) const {

	unsigned index, mask;
	std::tie(index, mask) = index_mask(pos, __func__);

	return _bits[index] & mask; 
}


bool bitvector::any() const {
	return std::any_of(_bits.begin(), _bits.end(), logical_true<size_t>());
}

bool bitvector::none() const {
	return std::none_of(_bits.begin(), _bits.end(), logical_true<size_t>());
}

bool bitvector::all() const {
	return std::all_of(_bits.begin(), _bits.end(), [](size_t x) { return x == NegativeOne; });
}

size_t bitvector::count() const {
	return std::accumulate(_bits.begin(), _bits.end(), 0, [](size_t a, size_t x){
		unsigned pop = 0;
		for(; x; x >>= 1) { pop += (x & 0x01); }
		return 0 + pop;
	});
}

size_t bitvector::size() const noexcept {
	return _bits.size() / sizeof(size_t);
}



bitvector &bitvector::flip() noexcept {
	std::transform(_bits.begin(), _bits.end(), _bits.begin(), bit_negate<size_t>());
	return *this;
}

bitvector& bitvector::flip( size_t pos ) {

	unsigned index, mask;
	std::tie(index, mask) = index_mask(pos, __func__);

	if (_bits[index] & mask) _bits[index] &= ~mask;
	else _bits[index] |= mask;

	return *this;
}


bitvector& bitvector::reset() noexcept {
	std::fill(_bits.begin(), _bits.end(), 0);
	return *this;
}

bitvector& bitvector::reset( size_t pos ) {
	unsigned index, mask;
	std::tie(index, mask) = index_mask(pos, __func__);

	_bits[index] &= ~mask;

	return *this;	
}


bitvector& bitvector::set() noexcept {
	std::fill(_bits.begin(), _bits.end(), NegativeOne);
	return *this;
}

bitvector& bitvector::set(size_t pos, bool value) {
	unsigned index, mask;
	std::tie(index, mask) = index_mask(pos, __func__);

	if (value) _bits[index] |= mask;
	else _bits[index] &= ~mask;

	return *this;
}


std::vector<size_t> bitvector::to_vector() {
	std::vector<size_t> tmp;

	size_t offset = 0;
	for (auto x : _bits) {

		unsigned bit = 0;
		for(; x ; x >>= 1, bit += 1) {
			if (x & 0x01) tmp.push_back(offset + bit);
		}

		offset += sizeof(size_t);
	}

	return tmp;
}

