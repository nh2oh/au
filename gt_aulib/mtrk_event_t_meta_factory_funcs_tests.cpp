#include "gtest/gtest.h"
#include "..\aulib\input\midi\mtrk_event_t.h"
#include <vector>
#include <cstdint>


// 
// mtrk_event_t make_tempo(const uint32_t& dt, const uint32_t& uspqn);
//
TEST(mtrk_event_t_meta_factories, makeTempo) {
	struct test_t {
		uint32_t dt_in {0};
		uint32_t tempo_in {0};
		uint32_t dt_ans {0};
		uint32_t tempo_ans {0};
	};

	std::vector<test_t> tests {
		{0, 0, 0, 0},
		{1, 1, 1, 1},
		{128, 1523, 128, 1523},
		{125428, 1523, 125428, 1523},
		// 16777215 == 0x00'FF'FF'FF is the largest possible 24-bit int
		{1,16777215,1,16777215}, 
		// In the next two, the value for tempo_in exceeds the max value, so
		// will be truncated to the max value of 16777215.  
		{1,16777216,1,16777215}, 
		{1,0xFFFFFFFFu,1,16777215},
		
	};
	for (const auto& e : tests) {
		auto ev = make_tempo(e.dt_in,e.tempo_in);
		EXPECT_TRUE(ev.is_small());
		EXPECT_FALSE(ev.is_big());
		EXPECT_EQ(ev.type(),smf_event_type::meta);
		EXPECT_TRUE(is_tempo(ev));
		EXPECT_EQ(ev.delta_time(),e.dt_ans);
		EXPECT_EQ(get_tempo(ev),e.tempo_ans);
	}
}
