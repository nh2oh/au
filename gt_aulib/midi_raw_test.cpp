#include "gtest/gtest.h"
#include "..\aulib\input\midi\midi_raw.h"
#include "..\aulib\input\midi\generic_chunk_low_level.h"
#include "..\aulib\input\midi\midi_vlq.h"
#include "..\aulib\input\midi\midi_delta_time.h"
#include <vector>
#include <array>
#include <cstdint>

// TODO:  Many (all?) of these are tests of things now in dbklib; tests should
// move to dbklib.  


TEST(midi_raw_tests, toBEByteOrder) {
	auto x = to_nearest_valid_delta_time(1575);

	struct test_t {
		uint32_t input {0};
		uint32_t ans {};
	};
	std::vector<test_t> tests {
		{0,0},
		{1,16777216},  // 0x00,00,00,01  =>  0x01,00,00,00
		{0x01000000u,0x00000001u},
		{0xFFFFFFFFu,0xFFFFFFFFu},
		{0x000C0000u, 0x00000C00u},
		{0x12345678u, 0x78563412u}
	};

	for (const auto& e : tests) {
		auto res = to_be_byte_order(e.input);
		EXPECT_EQ(res,e.ans) << "Failed for e.ans==" << e.ans << "\n";
	}
}


// Big-endian encoded multi-byte ints
TEST(midi_raw_tests, be2native) {
	struct i32_tests {
		std::array<unsigned char,4> field {0x00,0x00,0x00,0x00};
		int32_t ans {};
	};

	std::vector<i32_tests> i32_tests {
		{{0x00,0x00,0x00,0x00},0x00},
		{{0x40,0x00,0x00,0x00},0x40000000},
		{{0x00,0x00,0x00,0x40},0x00000040},
		{{0x7F,0x00,0x00,0x00},0x7F000000},
		{{0x00,0x00,0x00,0x7F},0x0000007F},

		{{0x71,0x00,0x00,0x00},0x71000000},
		{{0x70,0x00,0x00,0x00},0x70000000},
		{{0x0F,0x7F,0x00,0x00},0x0F7F0000},
		{{0x7F,0xFF,0x00,0x00},0x7FFF0000},

		{{0x71,0x80,0x00,0x00},0x71800000},
		{{0x00,0xC0,0x80,0x00},0x00C08000},
		{{0x3F,0xFF,0x7F,0x00},0x3FFF7F00},
		{{0x00,0x7F,0xFF,0x3F},0x007FFF3F},
	};
	for (const auto& e : i32_tests) {
		auto res = read_be<uint32_t>(e.field.begin(),e.field.end());
		EXPECT_EQ(res,e.ans) << "Failed for e.ans==" << e.ans << "\n";
	}

	struct ui64_tests {
		std::array<unsigned char,8> field {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
		uint64_t ans {};
	};
	std::vector<ui64_tests> ui64_tests {
		{{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},0x00},
		{{0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00},0x4000000000000000},
		{{0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x00},0x0000004000000000},
		{{0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00},0x7F00000000000000},
		{{0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00},0xFF00000000000000},
		{{0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x00},0x0000007F00000000},

		{{0x00,0x7F,0xFF,0x3F,0x00,0x00,0x00,0x00},0x007FFF3F00000000},
		{{0x00,0x00,0x00,0x00,0x00,0x7F,0xFF,0x3F},0x00000000007FFF3F},
	};
	for (const auto& e : ui64_tests) {
		auto res = read_be<uint64_t>(e.field.begin(),e.field.end());
		EXPECT_EQ(res,e.ans) << "Failed for e.ans==" << e.ans << "\n";
	}
}


// ...
TEST(midi_raw_tests, MIDIVLFieldEquivValueTests) {
	uint32_t a {137};
	uint32_t a_ans = 33033;
	EXPECT_EQ(a_ans,vlq_field_literal_value(a));

	uint32_t b {106903};
	uint32_t b_ans = 8831767;
	EXPECT_EQ(b_ans,vlq_field_literal_value(b));

	uint32_t c {268435455};  // 0x0F'FF'FF'FF 
	uint32_t c_ans = 4294967167;  // 0xFF'FF'FF'7F 
	EXPECT_EQ(c_ans,vlq_field_literal_value(c));

	uint32_t d = 255;
	uint32_t d_ans = 0x817F; 
	EXPECT_EQ(d_ans,vlq_field_literal_value(d));
}


// All the examples from p.131 of the MIDI std.  
// Note that the 'field' array in the test struct is always 4 long; 
// midi_interpret_vl_field() has to be able to autodetect the end.  
TEST(midi_raw_tests, VLFieldSize) {

	std::vector<uint32_t> onebyte_ui32t {0x00u,0x40u,0x7Fu};
	for (const auto& e : onebyte_ui32t) {
		auto res = vlq_field_size(e);
		EXPECT_EQ(res,1);
	}
	std::vector<uint8_t> onebyte_ui8t {0x00u,0x40u,0x7Fu};
	for (const auto& e : onebyte_ui8t) {
		auto res = vlq_field_size(e);
		EXPECT_EQ(res,1);
	}

	std::vector<uint32_t> twobyte_ui32t {0x80u,0x2000u,0x3FFFu};
	for (const auto& e : twobyte_ui32t) {
		auto res = vlq_field_size(e);
		EXPECT_EQ(res,2);
	}

	std::vector<uint32_t> threebyte_ui32t {0x4000u,0x100000u,0x1FFFFFu};
	for (const auto& e : threebyte_ui32t) {
		auto res = vlq_field_size(e);
		EXPECT_EQ(res,3);
	}

	std::vector<uint32_t> fourbyte_ui32t {0x00200000u,0x08000000u,0x0FFFFFFFu};
	for (const auto& e : fourbyte_ui32t) {
		auto res = vlq_field_size(e);
		EXPECT_EQ(res,4);
	}
}


// Similar to the VLFieldSize tests above, but checks the addnl fields
// returned by midi_interpret_vl_field() (.is_valid, .N).  
TEST(midi_raw_tests, midiInterpretVLFieldTests) {
	struct tests_t {
		std::array<unsigned char,6> field {0x00,0x00,0x00,0x00,0x00,0x00};
		vlq_field_interpreted ans {};
	};

	std::vector<tests_t> p131_tests {
		// From p131 of the MIDI std
		{{0x00,0x00,0x00,0x00,0x00,0x00},{0x00000000,1,true}},
		{{0x40,0x00,0x00,0x00,0x00,0x00},{0x00000040,1,true}},
		{{0x7F,0x00,0x00,0x00,0x00,0x00},{0x0000007F,1,true}},
		{{0xFF,0x7F,0x00,0x00,0x00,0x00},{0x00003FFF,2,true}},
		{{0x81,0x80,0x00,0x00,0x00,0x00},{0x00004000,3,true}},
		{{0xC0,0x80,0x00,0x00,0x00,0x00},{0x00100000,3,true}},
		{{0xFF,0xFF,0x7F,0x00,0x00,0x00},{0x001FFFFF,3,true}},
		{{0x81,0x80,0x80,0x00,0x00,0x00},{0x00200000,4,true}},
		{{0xC0,0x80,0x80,0x00,0x00,0x00},{0x08000000,4,true}},
		{{0xFF,0xFF,0xFF,0x7F,0x00,0x00},{0x0FFFFFFF,4,true}},
		
		// Does not read past a byte w/a leading 0
		{{0x00,0x80,0x80,0x80,0x00,0x00},{0x00000000,1,true}},

		// 0x80 bytes => 0
		{{0x80,0x80,0x00,0x00,0x00,0x00},{0x00000000,3,true}},
		{{0x80,0x80,0x80,0x00,0x00,0x00},{0x00000000,4,true}},
		{{0x80,0x80,0x80,0x70,0x00,0x00},{0x00000070,4,true}},

		// Should stop reading after 4 bytes
		{{0x80,0x80,0x80,0x80,0x80,0x80},{0x00000000,4,false}},
		{{0x80,0x80,0x80,0x70,0x80,0x80},{0x00000070,4,true}},

		// Last byte has a leading 1 => !valid
		{{0xC0,0x80,0x80,0x80,0x00,0x00},{0x08000000,4,false}},
		{{0x80,0x80,0x80,0x80,0x00,0x00},{0x00000000,4,false}},
		{{0xFF,0xFF,0xFF,0xFF,0x00,0x00},{0x0FFFFFFF,4,false}},

	};
	for (const auto& e : p131_tests) {
		auto res = read_vlq(e.field.begin(),e.field.end());
		EXPECT_EQ(res.val,e.ans.val); //<< "Failed for e.ans==" << e.ans << "\n";
		EXPECT_EQ(res.N,e.ans.N); //<< "Failed for e.ans==" << e.ans << "\n";
		EXPECT_EQ(res.is_valid,e.ans.is_valid);// << "Failed for e.ans==" << e.ans << "\n";
	}
}


// midi_write_vl_field() and all the examples from p.131 of the MIDI std.  
TEST(midi_raw_tests, WriteVLFieldStdP131Exs) {
	struct tests {
		std::array<unsigned char,4> ans {0x00,0x00,0x00,0x00};
		uint32_t num {};
	};
	std::vector<tests> all_tests {
		{{0x00,0x00,0x00,0x00},0x00},
		{{0x40,0x00,0x00,0x00},0x40},
		{{0x7F,0x00,0x00,0x00},0x7F},

		{{0x81,0x00,0x00,0x00},0x80},
		{{0xC0,0x00,0x00,0x00},0x2000},
		{{0xFF,0x7F,0x00,0x00},0x3FFF},

		{{0x81,0x80,0x00,0x00},0x4000},
		{{0xC0,0x80,0x00,0x00},0x100000},
		{{0xFF,0xFF,0x7F,0x00},0x1FFFFF},

		{{0x81,0x80,0x80,0x00},0x00200000},
		{{0xC0,0x80,0x80,0x00},0x08000000},
		{{0xFF,0xFF,0xFF,0x7F},0x0FFFFFFF}
	};
	for (const auto& e : all_tests) {
		std::array<unsigned char,4> curr_result {0x00u,0x00u,0x00u,0x00u};

		auto res = write_vlq(e.num,curr_result.begin());

		auto nbytes_written = res-curr_result.begin();
		EXPECT_EQ(nbytes_written,read_vlq(e.ans.begin(),e.ans.end()).N);

		for (int i=0; i<e.ans.size(); ++i) {  // always 4 iterations
			EXPECT_EQ(curr_result[i],e.ans[i]);
		}
		
	}
}


// midi_write_vl_field() and all the examples from p.131 of the MIDI std.  
// Tests for the overload taking a single output iterator
TEST(midi_raw_tests, WriteVLFieldBackInsertIteratorStdP131Exs) {
	struct tests {
		std::array<unsigned char,4> ans {0x00,0x00,0x00,0x00};
		uint32_t num {};
	};
	std::vector<tests> all_tests {
		{{0x00,0x00,0x00,0x00},0x00},
		{{0x40,0x00,0x00,0x00},0x40},
		{{0x7F,0x00,0x00,0x00},0x7F},

		{{0x81,0x00,0x00,0x00},0x80},
		{{0xC0,0x00,0x00,0x00},0x2000},
		{{0xFF,0x7F,0x00,0x00},0x3FFF},

		{{0x81,0x80,0x00,0x00},0x4000},
		{{0xC0,0x80,0x00,0x00},0x100000},
		{{0xFF,0xFF,0x7F,0x00},0x1FFFFF},

		{{0x81,0x80,0x80,0x00},0x00200000},
		{{0xC0,0x80,0x80,0x00},0x08000000},
		{{0xFF,0xFF,0xFF,0x7F},0x0FFFFFFF}
	};
	for (const auto& e : all_tests) {
		auto ans_n_bytes = read_vlq(e.ans.begin(),e.ans.end()).N;

		std::vector<unsigned char> curr_result {};
		auto res_it = write_vlq(e.num,std::back_inserter(curr_result));

		EXPECT_EQ(read_vlq(curr_result.begin(),curr_result.end()).N,ans_n_bytes);
		EXPECT_EQ(read_vlq(curr_result.begin(),curr_result.end()).val,e.num);
		for (int i=0; i<ans_n_bytes; ++i) {
			EXPECT_EQ(curr_result[i],e.ans[i]);
		}
	}
}


//
// Tests for:
// maybe_header_t read_chunk_header(const unsigned char *beg, 
//									const unsigned char *end);
//
// With inputs not valid as chunk headers
//
TEST(midi_raw_tests, ReadChunkHeaderInvalidChunkHeaders) {
	struct test_invalid_t {
		std::array<unsigned char,8> data {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
		uint32_t offset_to_data_end {0};
	};
	std::vector<test_invalid_t> tests_invalid_headers {
		// Invalid: Non-ASCII in ID field
		{{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},100},
		{{0x4D,0x7F,0x72,0x6B,0x00,0x00,0x00,0x00},100},  // 0x7F
		{{0x4D,0x80,0x72,0x6B,0x00,0x00,0x00,0x00},100},  // 0x80
		{{0x4D,0x1F,0x72,0x6B,0x00,0x00,0x00,0x00},100},  // 0x1F==31
		// Invalid: valid id, but length > max
		// max_length == 0x7FFFFFF7 == <int32_t>::max()-8
		// id == MTrk
		{{0x4D,0x54,0x72,0x6B,0x7F,0xFF,0xFF,0xF8},100},
		{{0x4D,0x54,0x72,0x6B,0xFF,0xFF,0xFF,0xFF},100},
		// Id == MThd
		{{0x4D,0x54,0x68,0x64,0x7F,0xFF,0xFF,0xF8},100},
		{{0x4D,0x54,0x68,0x64,0xFF,0xFF,0xFF,0xFF},100},
		// Invalid: valid id, but range < 8
		// Id == MTrk
		{{0x4D,0x54,0x72,0x6B,0x00,0x00,0x00,0x06},0},
		{{0x4D,0x54,0x72,0x6B,0x00,0x00,0x00,0x06},7},
		{{0x4D,0x54,0x72,0x6B,0x00,0x00,0x00,0x06},4},
		// Id == MThd
		{{0x4D,0x54,0x68,0x64,0x00,0x00,0x00,0x06},0},
		{{0x4D,0x54,0x68,0x64,0x00,0x00,0x00,0x06},7},
		{{0x4D,0x54,0x68,0x64,0x00,0x00,0x00,0x06},4}
	};

	for (const auto& tcase : tests_invalid_headers) {
		auto beg = tcase.data.data();
		auto end = beg + tcase.offset_to_data_end;
		auto header = read_chunk_header(beg,end);
		EXPECT_FALSE(header);
	}
}

//
// Tests for:
// maybe_header_t read_chunk_header(const unsigned char *beg, 
//									const unsigned char *end);
//
// With inputs valid as chunk headers
//
TEST(midi_raw_tests, ReadChunkHeaderValidChunkHeaders) {
	struct test_valid_t {
		std::array<unsigned char,8> data {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
		uint32_t offset_to_data_end {0};
		chunk_id ans_type {chunk_id::unknown};
		int32_t ans_length {0};
	};
	std::vector<test_valid_t> tests_valid_headers {
		// valid, MThd, MTrk
		// Length  == 0
		{{0x4D,0x54,0x68,0x64,0x00,0x00,0x00,0x00},100, chunk_id::mthd,0},
		{{0x4D,0x54,0x72,0x6B,0x00,0x00,0x00,0x00},100, chunk_id::mtrk,0},
		// Length == 6
		{{0x4D,0x54,0x68,0x64,0x00,0x00,0x00,0x06},100, chunk_id::mthd,6},
		{{0x4D,0x54,0x72,0x6B,0x00,0x00,0x00,0x06},100, chunk_id::mtrk,6},
		// max_length == 0x7FFFFFF7 == <int32_t>::max()-8
		// Note that:
		// data.begin()+8+length is > data.begin()+offset_to_data_end
		// But that's ok
		{{0x4D,0x54,0x68,0x64,0x7F,0xFF,0xFF,0xF7},100, chunk_id::mthd,0x7FFFFFF7},
		{{0x4D,0x54,0x72,0x6B,0x7F,0xFF,0xFF,0xF7},100, chunk_id::mtrk,0x7FFFFFF7},
		
		// id == unknown
		{{0x4D,0x54,0x4D,0x54,0x00,0x00,0x00,0x00},100, chunk_id::unknown,0},
		{{0x4D,0x54,0x68,0x65,0x00,0x00,0x00,0x06},100, chunk_id::unknown,6},
		{{0x4D,0x54,0x70,0x6B,0x00,0x00,0x00,0x00},100, chunk_id::unknown,0},
		{{0x4E,0x53,0x72,0x6B,0x00,0x00,0x00,0x06},100, chunk_id::unknown,6},

		// From random MIDI Files i have laying around
		{{0x4D,0x54,0x68,0x64,0x00,0x00,0x00,0x06},5584, chunk_id::mthd,6},
		{{0x4D,0x54,0x72,0x6B,0x00,0x00,0x00,0x1C},5570, chunk_id::mtrk,28},
		{{0x4D,0x54,0x72,0x6B,0x00,0x00,0x0F,0x13},5534, chunk_id::mtrk,3859},
		{{0x4D,0x54,0x72,0x6B,0x00,0x00,0x06,0x7B},1667, chunk_id::mtrk,1659},
	};
	for (const auto& tcase : tests_valid_headers) {
		auto beg = tcase.data.data();
		auto end = beg + tcase.offset_to_data_end;
		auto header = read_chunk_header(beg,end);
		EXPECT_TRUE(header);
		EXPECT_EQ(header.id,tcase.ans_type);
		EXPECT_EQ(header.length,tcase.ans_length);
	}
}





