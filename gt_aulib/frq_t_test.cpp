#include "gtest/gtest.h"
#include "..\aulib\types\frq_t.h"
#include <vector>
#include <string>
#include <cmath>

// 
TEST(frq_t_tests, DefaultConstructorTest) {
	// Default-constructed value is 1 Hz
	frq_t zero_hz {0};
	frq_t zeroargs {};
	EXPECT_TRUE(zeroargs == zero_hz);
}


