#include "gtest/gtest.h"
#include "..\aulib\melgen\randmel_gens.h"
#include "..\aulib\types\scd_t.h"
#include "dbklib\stats.h"
#include <random>

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
	auto re = new_randeng(true);
	std::uniform_int_distribution rd {};
	std::uniform_real_distribution rdd {};
	for (int i=0; i<50; ++i) {
		ma_params p {};
		rd.param(decltype(rd.param()) {10,100}); p.nnts = rd(re); //urandi(1,10,100)[0];
		rd.param(decltype(rd.param()) {50,200}); p.min = scd_t {rd(re)}; //scd_t{urandi(1,50,200)[0]};
		rd.param(decltype(rd.param()) {50,100}); p.max = p.min + scd_t {rd(re)}; //scd_t{urandi(1,50,100)[0]};
		rd.param(decltype(rd.param()) {1,1000}); p.npass = rd(re); //urandi(1,1,1000)[0];
		
		rdd.param(decltype(rdd.param()) {-10,10}); p.sc_adjnts = rdd(re); //urandd(1,-10,10)[0];
		p.sc_rptnts = rdd(re); //urandd(1,-10,10)[0];
		p.sc_stepsize = rdd(re); //urandd(1,-10,10)[0];
		rdd.param(decltype(rdd.param()) {0,10}); p.optimstep = rdd(re); //urandd(1,0,10)[0]; // should always be >= 0

		auto ma = melody_a(p);

		EXPECT_TRUE(ma.size() == p.nnts);
		EXPECT_TRUE(*std::max_element(ma.begin(), ma.end()) <= p.max);
		EXPECT_TRUE(*std::min_element(ma.begin(), ma.end()) >= p.min);
	}
}

