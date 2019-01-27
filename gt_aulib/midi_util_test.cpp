#include "gtest/gtest.h"
#include "..\aulib\input\midi_util.h"
#include <vector>
#include <array>


// All the examples from p.131 of the MIDI std
// Note that the 'field' array in the test struct is always 4 long; 
// midi_interpret_vl_field() has to be able to autodetect the end.  
TEST(midi_util_tests, VLFieldInterpretMIDIStdP131Examples) {
	struct tests {
		std::array<unsigned char,4> rep {0x00,0x00,0x00,0x00};
		int32_t ans {};
	};
	std::vector<tests> ln1_set {
		{{0x00,0x00,0x00,0x00},0x00},{{0x40,0x00,0x00,0x00},0x40},{{0x7F,0x00,0x00,0x00},0x7F}
	};
	for (const auto& e : ln1_set) {
		auto res = midi_interpret_vl_field(&(e.rep[0]));
		EXPECT_EQ(res.N,1);
		EXPECT_EQ(res.val,e.ans);
	}

	std::vector<tests> ln2_set {
		{{0x81,0x00,0x00,0x00},0x80},{{0xC0,0x00,0x00,0x00},0x2000},{{0xFF,0x7F,0x00,0x00},0x3FFF}
	};
	for (const auto& e : ln2_set) {
		auto res = midi_interpret_vl_field(&(e.rep[0]));
		EXPECT_EQ(res.N,2);
		EXPECT_EQ(res.val,e.ans);
	}

	std::vector<tests> ln3_set {
		{{0x81,0x80,0x00,0x00},0x4000},{{0xC0,0x80,0x00,0x00},0x100000},{{0xFF,0xFF,0x7F,0x00},0x1FFFFF}
	};
	for (const auto& e : ln3_set) {
		auto res = midi_interpret_vl_field(&(e.rep[0]));
		EXPECT_EQ(res.N,3);
		EXPECT_EQ(res.val,e.ans);
	}

	std::vector<tests> ln4_set {
		{{0x81,0x80,0x80,0x00},0x00200000},{{0xC0,0x80,0x80,0x00},0x08000000},{{0xFF,0xFF,0xFF,0x7F},0x0FFFFFFF}
	};
	for (const auto& e : ln4_set) {
		auto res = midi_interpret_vl_field(&(e.rep[0]));
		EXPECT_EQ(res.N,4);
		EXPECT_EQ(res.val,e.ans);
	}
}

