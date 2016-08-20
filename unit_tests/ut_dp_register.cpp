#include "dp_register.h"
#include "dp_register_set.h"

#include "catch.hpp"


dp_register operator "" _p(unsigned long long i) { return dp_register({'p', (unsigned int)i}); }
dp_register operator "" _r(unsigned long long i) { return dp_register({'r', (unsigned int)i}); }
dp_register operator "" _t(unsigned long long i) { return dp_register({'t', (unsigned int)i}); }
dp_register operator "" _v(unsigned long long i) { return dp_register({'v', (unsigned int)i}); }


TEST_CASE("dp_register_set::includes", "[dp_register_set]") {

  dp_register_set rs =  { 0_r, 128_r };


  CHECK(rs.includes(0_r) == true);
  CHECK(rs.includes(128_r) == true);
  CHECK(rs.includes(120_r) == false);

}

TEST_CASE("dp_register", "[dp_register]") {

	CHECK(123_r == 123_r);
	CHECK(0_r != 1_r);
	CHECK(0_r != 0_p);
	CHECK(0_r != 0_t);
	CHECK(0_r != 0_v);

	CHECK(++(0_r) == 2_r);
}
