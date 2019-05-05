#include "gtest/gtest.h"
#include "..\aulib\input\midi\midi_raw.h"
#include "..\aulib\input\midi\mthd_container_t.h"
#include <vector>
#include <cstdint>


// 
// From p.133 of the midi std
//
TEST(mthd_container_tests, interpretSMPTEField) {
	struct tests_t {
		uint16_t input {0};
		midi_smpte_field ans {};
	};

	std::vector<tests_t> tests {
		{0xE250u,{-30,80}},  // p.133 of the midi std
		{0xE728u,{-25,40}},   // 25fr/sec * 40tk/fr => 1000tk/sec => ms resolution
		{0xE350u,{-29,80}}
	};

	for (const auto& e : tests) {
		auto curr_ft = detect_midi_time_division_type(e.input);
		EXPECT_EQ(curr_ft,midi_time_division_field_type_t::SMPTE);

		auto curr = interpret_smpte_field(e.input);
		EXPECT_EQ(curr.time_code_fmt,e.ans.time_code_fmt);
		EXPECT_EQ(curr.units_per_frame,e.ans.units_per_frame);
	}

}

// 