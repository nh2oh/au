#include "gtest/gtest.h"
#include "..\aulib\types\scd_t.h"
#include <vector>

TEST(scd_t_tests, Set1FromMatlabImpl) {
	int N = 12;
	std::vector<int> vi_scd {0,1,2,15,-4,-16};
	std::vector<int> vi_rscd {0,1,2,3,8,8};

	for (int i=0; i<vi_scd.size(); ++i) {
		scd_t curr_scd {vi_scd[i]};
		rscdoctn_t curr_rscd {curr_scd,N};
		EXPECT_EQ(vi_rscd[i],curr_rscd.to_int());
	}

}


