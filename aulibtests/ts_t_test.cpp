#include "pch.h"
#include "..\aulib\types\ts_t.h"
#include <vector>
#include <string>
#include <cmath>
#include <limits>

/*
// Googletest does not support c++17, so i can't include my 
// header defining these functions.  
bool aprx_eq_gtest(double a, double b, int ulp=2) {
	auto e = std::numeric_limits<double>::epsilon();
	auto m = std::numeric_limits<double>::min();
	auto d = std::abs(a-b);
	auto s = std::abs(a+b);
	return (d <= e*s*ulp || d < m);
};

bool aprx_int_gtest(double a, int ulp=2) {
	double ra {std::round(a)};
	return aprx_eq_gtest(a, ra, ulp);
};*/

// No phase shift
TEST(ts_t_tests, DefaultConstructors) {
	EXPECT_TRUE(true);
}


