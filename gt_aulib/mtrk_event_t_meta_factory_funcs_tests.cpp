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
// mtrk_event_t make_{instname,lyric,marker,cuepoint,text,
//						copyright}(const uint32_t& dt, const std::string& s)
// ... All maker-functions for meta events with text payloads
//
TEST(mtrk_event_t_meta_factories, makeEventsWithTextPayloads) {
	struct testset_t {
		mtrk_event_t (*fp_make)(const uint32_t&, const std::string&);
		bool (*fp_is)(const mtrk_event_t&);
		meta_event_t ans_evtype;
	};
	std::vector<testset_t> testsets {
		{make_text,is_text,meta_event_t::text},
		{make_copyright,is_copyright,meta_event_t::copyright},
		{make_trackname,is_trackname,meta_event_t::trackname},
		{make_instname,is_instname,meta_event_t::instname},
		{make_lyric,is_lyric,meta_event_t::lyric},
		{make_marker,is_marker,meta_event_t::marker},
		{make_cuepoint,is_cuepoint,meta_event_t::cuepoint}		
	};

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
	for (const auto curr_testset : testsets) {
		for (const auto& e : tests) {
			auto ev = (*curr_testset.fp_make)(e.dt,e.s);
			EXPECT_EQ(ev.type(), smf_event_type::meta);
			EXPECT_EQ(classify_meta_event(ev),curr_testset.ans_evtype);
			EXPECT_TRUE((*curr_testset.fp_is)(ev));
			EXPECT_TRUE(meta_has_text(ev));
			EXPECT_EQ(ev.is_small(),e.issmall);
			EXPECT_NE(ev.is_big(),e.issmall);
			EXPECT_EQ(ev.delta_time(),e.dt);
			EXPECT_EQ(meta_generic_gettext(ev),e.s);
		}
	}
}
