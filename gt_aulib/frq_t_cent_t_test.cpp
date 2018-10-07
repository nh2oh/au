#include "gtest/gtest.h"
#include "..\aulib\types\frq_t.h"
#include <vector>
#include <string>
#include <cmath>

// 
TEST(frq_t_tests, DefaultConstructorTest) {
	// Default-constructed value is 1 Hz
	frq_t one_hz {1,frq_t::unit::Hz};
	frq_t zeroargs {};
	EXPECT_TRUE(zeroargs == one_hz);
}

// Negative values "round" to 0
TEST(frq_t_tests, NoNegativeValues) {
	frq_t zero_hz {0,frq_t::unit::Hz};
	frq_t one_hz {1,frq_t::unit::Hz};

	frq_t neg_one_hz {-1,frq_t::unit::Hz};
	EXPECT_TRUE(neg_one_hz == zero_hz);
	EXPECT_TRUE(-1*one_hz == zero_hz);
	EXPECT_TRUE(-2.3*one_hz == zero_hz);

	EXPECT_TRUE((one_hz-one_hz) == zero_hz);
}

// 
TEST(frq_t_tests, Units) {
	frq_t one_hz {1,frq_t::unit::Hz};
	frq_t one_khz {1,frq_t::unit::kHz};
	frq_t one_mhz {1,frq_t::unit::MHz};
	frq_t one_ghz {1,frq_t::unit::GHz};

	EXPECT_TRUE(one_khz == 1000*one_hz);
	EXPECT_TRUE(one_mhz == 1000*1000*one_hz);
	EXPECT_TRUE(one_ghz == 1000*1000*1000*one_hz);
}


