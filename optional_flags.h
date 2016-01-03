
#include <experimental/optional>
#include <unordered_map>

using namespace std::experimental;


class optional_flags {
	
public:

	optional_flags() = default;
	optional_flags(const optional_flags &) = default;

	optional_flags &operator=(const optional_flags &) = default;

	bool operator==(const optional_flags &rhs) const {
		return engaged == rhs.engaged && (flags & engaged) == (rhs.flags && engaged);
	}

	bool operator!=(const optional_flags &rhs) const {
		return engaged != rhs.engaged || (flags & engaged) != (rhs.flags && engaged);
	}

	bool operator<(const optional_flags &rhs) const {
		if (engaged == rhs.engaged) return (flags & engaged) < (rhs.flags & engaged);
		return engaged < rhs.engaged;
	}

	void reset(uint16_t mask) {
		engaged &= ~mask;
	}

	struct proxy {

		proxy &operator=(bool tf) {
			if (tf) parent.flags | mask;
			else parent.flags &= ~mask;
			parent.engaged |= mask;
			return *this;
		}

		proxy &operator=(const std::experimental::nullopt_t) {
			parent.engaged &= ~mask;
			return *this;
		}

		operator bool() const {
			return parent.engaged & mask;
		}

		bool operator!() const {
			return !(parent.engaged & mask);
		}

		bool operator*() const {
			return parent.flags & mask;
		}

		bool value() const {
			if (parent.engaged & mask)
				return parent.flags & mask;
			throw std::experimental::bad_optional_access();
		}

		bool value_or(bool tf) const {
			if (parent.engaged & mask)
				return parent.flags & mask;
			return tf;
		}

	private:
		friend class optional_flags;

		proxy(optional_flags &p, unsigned m) : parent(p), mask(m)
		{}

		optional_flags &parent;
		unsigned mask;

	};

	proxy c() { return proxy(*this, 1 << 0); }
	proxy z() { return proxy(*this, 1 << 1); }
	proxy i() { return proxy(*this, 1 << 2); }
	proxy d() { return proxy(*this, 1 << 3); }
	proxy x() { return proxy(*this, 1 << 4); }
	proxy m() { return proxy(*this, 1 << 5); }
	proxy v() { return proxy(*this, 1 << 6); }
	proxy n() { return proxy(*this, 1 << 7); }
	proxy e() { return proxy(*this, 1 << 8); }

	const proxy c() const { return proxy(*this, 1 << 0); }
	const proxy z() const { return proxy(*this, 1 << 1); }
	const proxy i() const { return proxy(*this, 1 << 2); }
	const proxy d() const { return proxy(*this, 1 << 3); }
	const proxy x() const { return proxy(*this, 1 << 4); }
	const proxy m() const { return proxy(*this, 1 << 5); }
	const proxy v() const { return proxy(*this, 1 << 6); }
	const proxy n() const { return proxy(*this, 1 << 7); }
	const proxy e() const { return proxy(*this, 1 << 8); }


private:
	friend struct proxy;
	uint16_t flags = 0;
	uint16_t engaged = 0;
};