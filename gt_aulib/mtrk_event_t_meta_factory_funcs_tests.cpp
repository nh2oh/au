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
		//{0, 0, 0, 0},
		{1, 1, 1, 1},
		{128, 1523, 128, 1523}
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
