#ifndef __variant__
#define __variant__

#include <type_traits>
#include <typeinfo>
#include <typeindex>
#include <stdexcept>
#include <cstdio>

template<class Type>
struct variant_helper {
	static void fx(int code, void *buffer, void *rhs) {
		typedef std::remove_reference_t<Type> T;
		if (code == 1) {
			((T *)buffer)->~T();
			return;
		}
		if (code == 2) {
			// copy constructor.
			new (buffer) T(*(const T*)rhs);
			return;
		}
		if (code == 3) {
			// move constructor.
			new (buffer) T(std::move(*(const T*)rhs));
			return;
		}
	}
};

class bad_variant_access : public std::logic_error {
public:
  explicit bad_variant_access(const std::string& what_arg) : logic_error(what_arg) {}
  explicit bad_variant_access(const char* what_arg) : logic_error(what_arg) {}
};


template <class T> struct emplaced_type_t{};
template <class T> constexpr emplaced_type_t<T> emplaced_type{};
template <size_t I> struct emplaced_index_t{};
template <size_t I> constexpr emplaced_index_t<I> emplaced_index;


template<class...>
struct variant_in_type : public std::false_type
{};

template<class A, class B>
struct variant_in_type<A, B> : public std::is_same<A, std::remove_reference_t<B>>
{};

template<class A, class H, class... Tail>
struct variant_in_type<A, H, Tail...> : public
std::conditional<
	std::is_same<A, std::remove_reference_t<H>>::value,
	std::true_type,
	variant_in_type<A, Tail...>
>::type
{};


template<class... Types>
class variant {
	
	public:

	variant() : __type_info(typeid(void))
	{}

	template<class Type, class... Args>
	variant(emplaced_type_t<Type>, Args&& ...args) : __type_info(typeid(std::remove_reference_t<Type>)) {
		typedef std::remove_cv_t<std::remove_reference_t<Type>> T;

		static_assert(variant_in_type<T, Types...>::value, "Invalid type for variant.");

		__fx = variant_helper<T>::fx;
		new (&__buffer) T (std::forward<Args>(args)...);
	}

	~variant() {
		if (__fx) __fx(1, &__buffer, nullptr);
	}

	variant(const variant &rhs) : __type_info(rhs.__type_info) {
		__fx = rhs.__fx;
		if (__fx) __fx(2, &__buffer, (void *)&rhs.__buffer);
	}

	variant(variant &&rhs) : __type_info(rhs.__type_info) {
		__fx = rhs.__fx;
		if (__fx) {
			__fx(3, &__buffer, &rhs.__buffer);
			rhs.disengage();
		}
	}

	variant &operator=(const variant &) = delete;
	variant &&operator=(variant &&) = delete;


	private:

		template<class T, class... xTypes>
		friend const std::remove_reference_t<T> &get(const variant<xTypes...> &);
		template<class T, class... xTypes>
		friend std::remove_reference_t<T> &&get(variant &&);

		void disengage() {
			if (__fx) {
				__fx(1, &__buffer, nullptr);
				__fx = nullptr;
				__type_info = typeid(void);
			}
		}
		void (*__fx)(int, void *, void *) = nullptr;
		std::type_index __type_info;
		typename std::aligned_union<1,Types...>::type __buffer;
};

template<class Type, class... Types>
const std::remove_reference_t<Type> & get(const variant<Types...> & v) {
	typedef std::remove_reference_t<Type> T;

	static_assert(variant_in_type<T, Types...>::value, "Invalid type for variant.");


	auto ti = std::type_index(typeid(T));

	if (v.__type_info != ti) {
		fprintf(stderr, "%s : %s\n", v.__type_info.name(), ti.name());
		throw bad_variant_access(ti.name());
	}
	const T *tmp = (const T *)&v.__buffer;
	return *tmp;
}

template<class Type, class... Types>
inline std::remove_reference_t<Type> && get(variant<Types...> && v) {
	typedef std::remove_reference_t<Type> T;

	static_assert(variant_in_type<T, Types...>::value, "Invalid type for variant.");


	auto ti = std::type_index(typeid(T));

	if (v.__type_info != ti) {
		fprintf(stderr, "%s : %s\n", v.__type_info.name(), ti.name());
		throw bad_variant_access(ti.name());
	}
	T *tmp = (T *)&v.__buffer;
	return *tmp;
}


#endif


