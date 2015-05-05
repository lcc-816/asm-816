
#ifndef __bitvector_h__
#define __bitvector_h__

#include <vector>
#include <tuple>

class bitvector {
	
	public:

	bitvector() = default;
	bitvector(const bitvector &) = default;
	bitvector(bitvector &&) = default;

	bitvector &operator=(const bitvector &) = default;
	bitvector &operator=(bitvector &&) = default;

	bool operator==(const bitvector &rhs) const;
	bool operator!=(const bitvector &rhs) const;


	struct reference {
		reference& operator=( const reference& x ) = default;

		reference& operator=( bool x ) {
			_bv.set(_pos, x);
			return *this;
		}

		operator bool() const noexcept {
			return _bv[_pos];
		}

		bool operator~() const noexcept {
			return !_bv[_pos];
		}

		reference& flip() noexcept {
			_bv.flip(_pos);
			return *this;
		}

	private:
		friend class bitvector;
		reference(bitvector &bv, size_t pos) : _bv(bv), _pos(pos) {}
		bitvector &_bv;
		size_t _pos;
	};




	bool operator[]( size_t pos ) const {
		unsigned index = pos / sizeof(size_t);
		unsigned mask = 1 << (pos % sizeof(size_t));
		return _bits[index] & mask;
	}

	reference operator[]( size_t pos ) {
		return reference(*this, pos);
	}

	bool test( size_t pos ) const;


	bool any() const;
	bool none() const;
	bool all() const;

	size_t count() const;
	size_t size() const noexcept;

	bitvector& operator&=( const bitvector& other );
	bitvector& operator|=( const bitvector& other );
	bitvector& operator^=( const bitvector& other );
	bitvector operator~() const;

	// missing bit ops
	bitvector& bit_nor( const bitvector& other );   // ~(p | q)
	bitvector& bit_lt( const bitvector& other );    // ~p & q
	bitvector& bit_gt( const bitvector& other );    // p & ~q
	bitvector& bit_nand( const bitvector& other );  // ~(p & q)
	bitvector& bit_eq( const bitvector& other );    // ~(p ^ q)
	bitvector& bit_le( const bitvector& other );    // ~p | q
	bitvector& bit_ge( const bitvector& other );    // p | ~q


	/*
	bitvector operator<<( std::size_t pos ) const;
	bitvector& operator<<=( std::size_t pos );
	bitvector operator>>( std::size_t pos ) const;
	bitvector& operator>>=( std::size_t pos );
	*/

	bitvector& set() noexcept;
	bitvector& set( size_t pos, bool value = true );

	bitvector& reset() noexcept;
	bitvector& reset( size_t pos );

	bitvector& flip() noexcept;
	bitvector& flip( size_t pos );



	//void shrink_to_fit();
	std::vector<size_t> to_vector(); // return a vector of set bits

	private:
	// size_t _size ? 
	std::vector<size_t> _bits;

	std::tuple<unsigned, unsigned> index_mask(size_t pos, const char *name) const;
	std::tuple<unsigned, unsigned> index_mask(size_t pos) const noexcept;
};

inline bitvector operator&( const bitvector& lhs, const bitvector& rhs ) {
	bitvector tmp = lhs;
	tmp &= rhs;
	return tmp;
}

inline bitvector operator|( const bitvector& lhs, const bitvector& rhs ) {
	bitvector tmp = lhs;
	tmp |= rhs;
	return tmp;
}

inline bitvector operator^( const bitvector& lhs, const bitvector& rhs ) {
	bitvector tmp = lhs;
	tmp ^= rhs;
	return tmp;

}

#endif