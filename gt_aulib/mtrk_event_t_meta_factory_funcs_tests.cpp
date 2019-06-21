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
		EXPECT_EQ(classify_meta_event(ev),meta_event_t::tempo);
		EXPECT_TRUE(is_tempo(ev));
		EXPECT_EQ(ev.delta_time(),e.dt_ans);
		EXPECT_EQ(get_tempo(ev),e.tempo_ans);
	}
}


// 
// mtrk_event_t make_eot(const uint32_t& dt)
//
TEST(mtrk_event_t_meta_factories, makeEOT) {
	std::vector<uint32_t> tests {0,1,128,125428};
	for (const auto& e : tests) {
		auto ev = make_eot(e);
		EXPECT_TRUE(ev.is_small());
		EXPECT_FALSE(ev.is_big());
		EXPECT_EQ(ev.type(),smf_event_type::meta);
		EXPECT_EQ(classify_meta_event(ev),meta_event_t::eot);
		EXPECT_TRUE(is_eot(ev));
		EXPECT_EQ(ev.delta_time(),e);
	}
}


// 
// mtrk_event_t make_timesig(const uint32_t& dt, const midi_timesig_t& ts)
//
TEST(mtrk_event_t_meta_factories, makeTimesig) {
	struct test_t {
		uint32_t dt {0};
		midi_timesig_t ts {0,0,0,0};
	};

	std::vector<test_t> tests {
		{0, {0, 0, 0, 0}},
		{0, {6, 3, 36, 8}},  // From the midi std p. 10
		{1, {1, 1, 1, 1}},

		{128, {0, 0, 0, 0}},
		{128, {6, 3, 36, 8}},  // From the midi std p. 10
		{128, {1, 1, 1, 1}},

		{125428, {0, 0, 0, 0}},
		{125428, {6, 3, 36, 8}},  // From the midi std p. 10
		{125428, {1, 1, 1, 1}},
	};
	for (const auto& e : tests) {
		auto ev = make_timesig(e.dt,e.ts);
		EXPECT_TRUE(ev.is_small());
		EXPECT_FALSE(ev.is_big());
		EXPECT_EQ(ev.type(),smf_event_type::meta);
		EXPECT_EQ(classify_meta_event(ev),meta_event_t::timesig);
		EXPECT_TRUE(is_timesig(ev));
		EXPECT_EQ(ev.delta_time(),e.dt);
		EXPECT_EQ(get_timesig(ev),e.ts);
	}
}


// 
// mtrk_event_t make_instname(const uint32_t& dt, const std::string& s)
//
TEST(mtrk_event_t_meta_factories, makeInstname) {
	struct test_t {
		uint32_t dt {0};
		std::string s {};
		bool issmall {false};
	};

	std::vector<test_t> tests {
		{0, "Acoustic Grand", true},
		{0, "", true},
		{1, " ", true},
		{9, "     ", true},
		{0, "This string exceeds the size of the small buffer in mtrk_event_t.  ", false},
		{125428, "This string exceeds the size of the small buffer in mtrk_event_t.  ", false},
		{125428, "", true},
		{125428, "", true},
		// Maximum allowed dt
		{0x0FFFFFFFu, "", true},
		{0x0FFFFFFFu, "Acoustic Grand", true},  // From the midi std p. 10
		{0x0FFFFFFFu, "This string exceeds the size of the small buffer in mtrk_event_t.  ", false}
	};
	for (const auto& e : tests) {
		auto ev = make_instname(e.dt,e.s);
		EXPECT_EQ(ev.is_small(),e.issmall);
		EXPECT_NE(ev.is_big(),e.issmall);
		EXPECT_EQ(ev.type(),smf_event_type::meta);
		EXPECT_EQ(classify_meta_event(ev),meta_event_t::instname);
		EXPECT_TRUE(is_instname(ev));
		EXPECT_EQ(ev.delta_time(),e.dt);
		EXPECT_EQ(meta_generic_gettext(ev),e.s);
	}
}

