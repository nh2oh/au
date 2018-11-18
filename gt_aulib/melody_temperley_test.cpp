#include "gtest/gtest.h"
#include "..\aulib\melgen\randmel_gens.h"
#include "..\aulib\types\ntl_t.h"


TEST(melody_temperley_tests, CallWithDefaultParams) {
	melody_temperley_params p {};
	auto rm = melody_temperley(p);
	EXPECT_EQ(rm.size(), p.nnts);
}


