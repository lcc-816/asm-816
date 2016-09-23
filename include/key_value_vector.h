#include <vector>
#include <utility>

template<
    class Key,
    class T,
    class Compare = std::less<Key>,
    class Allocator = std::allocator<std::pair<const Key, T> >
> class key_value_vector {

private:
	typedef std::vector< std::pair< const Key, T> > __base;
	__base _v;
public:


	typedef Key key_type;
	typedef T mapped_type;
	typedef std::pair<const Key, T> value_type;
	typedef __base::size_type size_type;
	typedef __base::difference_type difference_type;
	typdef Compare key_compare;
	typedef Allocator allocator_type;
	typedef value_type& reference;

	typedef const value_type & const_reference;
	typedef std::allocator_traits<Allocator>::pointer pointer;
	typedef std::allocator_traits<Allocator>::const_pointer const_pointer;

	typedef __base::allocator_type iterator;
	typedef __base::allocator_type const_iterator;
	typedef __base::allocator_type reverse_iterator;
	typedef __base::allocator_type const_reverse_iterator;



	template< class... Args >
	reference emplace_back( Args&&... args ) {
		_v.emplace_back(std::forward<Args>(args)...);
		return _v.back();
	}

	// capacity.
	bool empty() const noexcept { return _v.empty(); }
	size_type capacity() const noexcept { return _v.capacity(); }
	size_type max_size() const noexcept { return _v.max_size(); }
	size_type size() const noexcept { return _v.size(); }
	void reserve( size_type new_cap ) { _v.reserve(new_cap); }
	void shrink_to_fit() { _v.shrink_to_fit(); }

	// iterators.
	iterator begin() noexcept { return _v.begin(); }
	const_iterator begin() const noexcept { return _v.begin(); }
	const_iterator cbegin() const noexcept { return _v.cbegin(); }

	iterator end() { return _v.end(); }
	const_iterator end() const noexcept { return _v.end(); }
	const_iterator cend() const noexcept { return _v.cend(); }

	reverse_iterator rbegin() noexcept { return _v.rbegin(); }
	const_reverse_iterator rbegin() const noexcept { return _v.rbegin(); }
	const_reverse_iterator crbegin() const noexcept { return _v.crbegin(); }

	reverse_iterator rend() noexcept { return _v.rend(); }
	const_reverse_iterator rend() const noexcept { return _v.rend(); }
	const_reverse_iterator crend() const noexcept { return _v.crend(); }



	iterator find(const Key &key) noexcept {
		return std::find_if(_v.begin(), _v.end(), [&key](const &value_type e) {
			return e.first == key;
		});
	}
	const_iterator find( const Key& key ) const noexcept {
		return std::find_if(_v.begin(), _v.end(), [&key](const &value_type e) {
			return e.first == key_type;
		});		
	}
	size_type count( const Key& key ) const;

	void clear() noexcept { _v.clear(); }
}