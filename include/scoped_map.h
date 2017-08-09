#ifndef __scoped_map_h__
#define __scoped_map_h__

#include <functional>
#include <utility>
#include <iterator>
#include <array>

template<class K, class V,
	class Hash = std::hash<K>,
	class Pred = std::equal_to<K>
>
class scoped_map
{
public:
	typedef K key_type;
	typedef V mapped_type;
	typedef std::pair<const K, V> value_type;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef Hash hasher;
	typedef Pred key_equal;
	//typedef allocator_type Allocator;

	typedef value_type& reference;
	typedef const value_type& const_reference;

	typedef value_type *pointer;
	typedef const value_type * const_pointer;

	class iterator;
	typedef const iterator const_iterator;

	enum { bucket_size = 997 };

private:


	struct node;
	struct super_node {
		node *next = nullptr;
		int generation = -1;

		super_node() = default;
		super_node(node *next, int generation) :
			next(next), generation(generation) {}

		super_node(const super_node &) = default;
		super_node &operator=(const super_node &) = default;

	};

	struct node : public super_node {

		template<class... Args>
		node(node *next, int generation, size_t hash, Args&&... args) :
			super_node(next, generation), hash(hash), data(std::forward<Args>(args)...)
		{}

		node(const node &) = delete;
		node &operator=(const node &) = delete;

		size_t hash = 0;
		value_type data;
	};

	struct spine {
		spine *next = nullptr;
		std::array<super_node, bucket_size> buckets;
	};


	int __generation = 0;
	spine *__spine = nullptr;


	void init() {
		/* initialize spine with pointers to next bucket */
		__spine = new spine;
		for (int i = 0; i < bucket_size-i; ++i) {
			__spine->buckets[i].next = (node *)&__spine->buckets[i+1];
		}
	}

	size_t hash(const key_type &key) const {
		hasher h;
		return h(key);
	}
	bool eq(const key_type &a, const key_type &b) const {
		key_equal eq;
		return eq(a, b);
	}


public:
	class iterator {
	public:
	    typedef std::forward_iterator_tag iterator_category;

		typedef std::pair<const K, V> value_type;
		typedef std::ptrdiff_t difference_type;
		typedef value_type& reference;
		typedef value_type *pointer;


		iterator(const iterator &) = default;
		iterator(iterator &&) = default;
		~iterator() = default;

		iterator &operator=(const iterator &rhs) = default;
		iterator &operator=(iterator &&rhs) = default;

		int generation() const { return ptr ? ptr->generation : -1; }

		iterator& advance() {
			if (ptr) {
				do {
					ptr = ptr->next;
				} while (ptr && ptr->generation < 0);
			}

			return *this;
		}
		reference operator*() const { return ptr->data; }
		pointer operator->() const { return &(ptr->data); }

		iterator operator++(int) { iterator tmp(*this); advance(); return tmp; }
		iterator operator++() { return advance(); }

		friend bool operator==(const iterator &lhs, const iterator &rhs)
		{ return lhs.ptr == rhs.ptr; }

		friend bool operator!=(const iterator &lhs, const iterator &rhs)
		{ return lhs.ptr != rhs.ptr; }

	private:
		friend scoped_map;
		iterator(node *n) : ptr(n) {}
		iterator() = default;

		node *ptr = nullptr;
	};


	scoped_map() = default;
	~scoped_map() {
		clear();
	}

	scoped_map(scoped_map &&rhs) {
		std::swap(__spine, rhs.__spine);
		std::swap(__generation, rhs.__generation);
	}

	int generation() { return __generation; }

	iterator find(const key_type key) {
		if (!__spine) return end();

		size_t h = hash(key);

		node *n = __spine->buckets[h % bucket_size].next;
		while (n && n->generation >= 0) {
			if (n->hash == h && eq(n->data.first, key)) {
				return iterator(n);
			}
			n = n->next;
		}
		return end();
	}

	iterator find_in_current_scope(const key_type key) {
		if (!__spine) return end();
		size_t h = hash(key);
		node *n = __spine->buckets[h % bucket_size].next;
		while (n && n->generation == __generation) {
			if (n->hash == h && eq(n->data.first, key)) {
				return iterator(n);
			}
			n = n->next;
		}
		return end();
	}

	std::pair<iterator,bool> insert( const value_type& value ) {

		if (!__spine) init();
		// 1. check if exists in current scope..

		size_t h = hash(value.first);
		auto &head = __spine->buckets[h % bucket_size];

		node *n = head.next;

		while (n && n->generation == __generation) {
			if (h == n->hash && eq(n->data.first, value.first)) {
				return std::make_pair(iterator(n), false);
			}
		}


		n = new node(head.next, __generation, h, value);
		head.next = n;
		return std::make_pair(iterator(n), true);
	}


	template<class... Args>
	std::pair<iterator,bool> emplace(Args&&... args) {


		/*
		 * possible optimization...
		 * keep a pointer to a node so failed emplace doesn't
		 * use any memory.
		 *
		 */

		if (!__spine) init();
		// 1. check if exists in current scope..

		// create a new node entry, then check if it already exists.
		node *nn = new node(nullptr, __generation, 0, std::forward<Args>(args)...);

		const auto &value = nn->data;
		size_t h = hash(value.first);
		auto &head = __spine->buckets[h % bucket_size];

		node *n = head.next;

		while (n && n->generation == __generation) {
			if (h == n->hash && eq(n->data.first, value.first)) {
				delete nn;
				return std::make_pair(iterator(n), false);
			}
		}

		nn->hash = h;
		nn->next = head.next;
		head.next = nn;
		return std::make_pair(iterator(nn), true);
	}



	size_type count( const key_type& key ) const {
		if (!__spine) return 0;

		size_t h = hash(key);
		node *n = __spine->buckets[h % bucket_size].next;
		while (n && n->generation >= 0) {
			if (n->hash == h && eq(n->data.first, key)) {
				return 1;
			}
			n = n->next;
		}
		return 0;	
	}


	iterator begin() {
		if (!__spine) return end();
		return iterator(__spine->buckets.front().next).advance();
	}

	const_iterator begin() const {
		if (!__spine) return end();
		return iterator(__spine->buckets.front().next).advance();
	}

	const_iterator cbegin() const {
		if (!__spine) return cend();
		return iterator(__spine->buckets.front().next).advance();
	}


	iterator end() {
		return iterator();
	}

	const_iterator end() const {
		return iterator();
	}

	const_iterator cend() const {
		return iterator();
	}


	void pop_scope() {
		if (!__spine) return;
		if (__generation == 0) return;

		int g = __generation--;

		for (auto &e : __spine->buckets) {
			node *n = e.next;
			while (n && n->generation == g) {
				auto tmp = n->next;
				delete n;
				n = tmp;
			}
		}
		auto prev = __spine->next;
		std::copy(prev->buckets.begin(), prev->buckets.end(), __spine->buckets.begin());
		__spine->next = prev->next;
		delete prev;
	}

	void push_scope() {

		if (!__spine) init();

		// since address of original buckets now important, need to stash original values and overwrite them...

		spine *sp = new spine;

		std::copy(__spine->buckets.begin(), __spine->buckets.end(), sp->buckets.begin());
		sp->next = __spine->next;
		__spine->next = sp;
		__generation++;
	}

	void clear() {
		if (!__spine) return;

		// destroy everything in current scope, then destroy all the spines.

		super_node *n = &__spine->buckets[0];
		while (n) {
			auto tmp = n->next;
			if (n->generation >= 0) {
				delete n;
			}
			n = tmp;
		}

		auto spine = __spine;
		while (spine) {
			auto tmp = spine->next;
			delete spine;
			spine = tmp;
		}

		__spine = nullptr;
		__generation = 0;
	}




};

#endif


