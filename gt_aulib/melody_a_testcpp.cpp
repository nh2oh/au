#include "gtest/gtest.h"
#include "..\aulib\melgen\randmel_gens.h"
#include "..\aulib\types\scd_t.h"
#include "..\aulib\util\au_random.h"

// ...
TEST(melody_a_tests, DefaultParams) {
	ma_params p {};
	auto ma = melody_a(p);

	EXPECT_TRUE(ma.size() == p.nnts);
	EXPECT_TRUE(*std::max_element(ma.begin(), ma.end()) <= p.max);
	EXPECT_TRUE(*std::min_element(ma.begin(), ma.end()) >= p.min);
}

// ...
TEST(melody_a_tests, RandomParams) {
	for (int i=0; i<50; ++i) {
		ma_params p {};
		p.nnts = urandi(1,10,100)[0];
		p.min = scd_t{urandi(1,50,200)[0]};
		p.max = p.min + scd_t{urandi(1,50,100)[0]};
		p.npass = urandi(1,1,1000)[0];
		p.sc_adjnts = urandd(1,-10,10)[0];
		p.sc_rptnts = urandd(1,-10,10)[0];
		p.sc_stepsize = urandd(1,-10,10)[0];
		p.optimstep = urandd(1,0,10)[0]; // should always be >= 0

		auto ma = melody_a(p);

		EXPECT_TRUE(ma.size() == p.nnts);
		EXPECT_TRUE(*std::max_element(ma.begin(), ma.end()) <= p.max);
		EXPECT_TRUE(*std::min_element(ma.begin(), ma.end()) >= p.min);
	}
}

