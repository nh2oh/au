#include "gtest/gtest.h"
#include "..\aulib\input\midi\midi_raw.h"
#include "..\aulib\input\midi\mthd_t.h"
#include <vector>
#include <cstdint>

TEST(mthd_container_tests, defaultCtor) {
	std::vector<unsigned char> ans {
		0x4Du,0x54u,0x68u,0x64u, 0x00u,0x00u,0x00u,0x06u,
		0x00u,0x01u, 0x00u,0x00u, 0x00u,0x78u
	};
	auto mthd = mthd_t();
	EXPECT_EQ(mthd.size(), ans.size());
	EXPECT_EQ(mthd.nbytes(), ans.size());
	
	auto p = mthd.data();
	auto it=mthd.begin(); auto cit=mthd.cbegin();
	auto end=mthd.end(); auto cend=mthd.cend();
	for (int i=0; i<mthd.size(); ++i) {
		EXPECT_EQ(*p++,ans[i]);
		EXPECT_EQ(mthd[i],ans[i]);
		EXPECT_NE(it,end);
		EXPECT_NE(cit,cend);
		EXPECT_EQ(*it++,ans[i]);
		EXPECT_EQ(*cit++,ans[i]);
	}
	EXPECT_EQ(it,end);
	EXPECT_EQ(cit,cend);

	EXPECT_EQ(mthd.length(),6);
	time_division_t tdf_ans(0x0078u);
	EXPECT_EQ(mthd.division().raw_value(),tdf_ans.raw_value());
	EXPECT_EQ(mthd.format(),1);
	EXPECT_EQ(mthd.ntrks(),0);
}


// 
// From p.133 of the midi std
//
TEST(mthd_container_tests, interpretSMPTEField) {
	struct test_t {
		uint16_t input {0};
		int8_t ans_tcf {0};
		uint8_t ans_upf {0};
	};
	std::vector<test_t> tests {
		{0xE250u,-30,80},  // p.133 of the midi std
		{0xE728u,-25,40},   // 25fr/sec * 40tk/fr => 1000tk/sec => ms resolution
		{0xE350u,-29,80}
	};

	for (const auto& e : tests) {
		time_division_t curr_tdf(e.input);
		EXPECT_EQ(type(curr_tdf),time_division_t::type::smpte);

		auto curr_tcf = get_time_code_fmt(curr_tdf);
		EXPECT_EQ(curr_tcf,e.ans_tcf);
		auto curr_upf = get_units_per_frame(curr_tdf);
		EXPECT_EQ(curr_upf,e.ans_upf);
	}

}

// 