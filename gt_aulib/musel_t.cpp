//#include "pch.h"
#include "..\aulib\types\musel_t.h"
#include "..\aulib\types\chord_t.h"
#include "..\aulib\types\scd_t.h"
#include <vector>
#include "gtest/gtest.h"


// ...
TEST(musel_t_tests, nameoftests) {
	EXPECT_TRUE(false);
	
	std::vector<scd_t> vs1 {scd_t{2},scd_t{13},scd_t{8},scd_t{10}};
	std::vector<scd_t> vs2 {scd_t{1},scd_t{3},scd_t{5},scd_t{7}};
	std::vector<scd_t> vs3 {scd_t{0},scd_t{2},scd_t{4},scd_t{6},scd_t{8}};
	chord_t<scd_t> c1 {vs1};
	chord_t<scd_t> c2 {vs2};
	chord_t<scd_t> c3 {vs3};

	musel_t<scd_t> d_c1 {c1,false};
	musel_t<scd_t> d_c2 {c2,false};
	musel_t<scd_t> d_c3 {c3,false};
	musel_t<scd_t> d_s1 {scd_t{4},false};
	
}



