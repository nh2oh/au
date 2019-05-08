#include "gtest/gtest.h"
#include "..\aulib\input\midi\midi_raw.h"
#include "dbklib\byte_manipulation.h"
#include <vector>
#include <array>
#include <cstdint>

// TODO:  Many (all?) of these are tests of things now in dbklib; tests should
// move to dbklib.  

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
		auto res = dbk::be_2_native<uint32_t>(&(e.field[0]));
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
		auto res = dbk::be_2_native<uint64_t>(&(e.field[0]));
		EXPECT_EQ(res,e.ans) << "Failed for e.ans==" << e.ans << "\n";
	}
}


// ...
TEST(midi_raw_tests, MIDIVLFieldEquivValueTests) {
	uint32_t a {137};
	uint32_t a_ans = 33033;
	EXPECT_EQ(a_ans,midi_vl_field_equiv_value(a));

	uint32_t b {106903};
	uint32_t b_ans = 8831767;
	EXPECT_EQ(b_ans,midi_vl_field_equiv_value(b));

	uint32_t c {268435455};  // 0x0F'FF'FF'FF 
	uint32_t c_ans = 4294967167;  // 0xFF'FF'FF'7F 
	EXPECT_EQ(c_ans,midi_vl_field_equiv_value(c));

	uint32_t d = 255;
	uint32_t d_ans = 0x817F; 
	EXPECT_EQ(d_ans,midi_vl_field_equiv_value(d));
}


// All the examples from p.131 of the MIDI std.  
// Note that the 'field' array in the test struct is always 4 long; 
// midi_interpret_vl_field() has to be able to autodetect the end.  
TEST(midi_raw_tests, VLFieldSize) {

	std::vector<int32_t> onebyte_i32t {0x00,0x40,0x7F};
	for (const auto& e : onebyte_i32t) {
		auto res = midi_vl_field_size(e);
		EXPECT_EQ(res,1);
	}
	std::vector<int8_t> onebyte_i8t {0x00,0x40,0x7F};
	for (const auto& e : onebyte_i8t) {
		auto res = midi_vl_field_size(e);
		EXPECT_EQ(res,1);
	}

	std::vector<int32_t> twobyte_i32t {0x80,0x2000,0x3FFF};
	for (const auto& e : twobyte_i32t) {
		auto res = midi_vl_field_size(e);
		EXPECT_EQ(res,2);
	}

	std::vector<int32_t> threebyte_i32t {0x4000,0x100000,0x1FFFFF};
	for (const auto& e : threebyte_i32t) {
		auto res = midi_vl_field_size(e);
		EXPECT_EQ(res,3);
	}

	std::vector<int32_t> fourbyte_i32t {0x00200000,0x08000000,0x0FFFFFFF};
	for (const auto& e : fourbyte_i32t) {
		auto res = midi_vl_field_size(e);
		EXPECT_EQ(res,4);
	}
	std::vector<uint32_t> fourbyte_ui32t {0x00200000,0x08000000,0x0FFFFFFF};
	for (const auto& e : fourbyte_ui32t) {
		auto res = midi_vl_field_size(e);
		EXPECT_EQ(res,4);
	}
}


// Similar to the VLFieldSize tests above, but checks the addnl fields
// returned by midi_interpret_vl_field() (.is_valid, .N).  
TEST(midi_raw_tests, midiInterpretVLFieldTests) {
	struct tests_t {
		std::array<unsigned char,6> field {0x00,0x00,0x00,0x00,0x00,0x00};
		midi_vl_field_interpreted ans {};
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
		auto res = midi_interpret_vl_field(&(e.field[0]));
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
		const unsigned char *p_res_field = &(curr_result[0]);
		const unsigned char *p_ans_field = &(e.ans[0]);

		auto res = midi_write_vl_field(curr_result.begin(),curr_result.end(),e.num);

		auto nbytes_written = res-curr_result.begin();
		EXPECT_EQ(nbytes_written,midi_interpret_vl_field(p_ans_field).N);

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
		const unsigned char *p_ans_field = &(e.ans[0]);
		auto ans_n_bytes = midi_interpret_vl_field(p_ans_field).N;

		std::vector<unsigned char> curr_result {};
		auto res_it = midi_write_vl_field(std::back_inserter(curr_result),e.num);
		const unsigned char *p_res_field = &(curr_result[0]);

		EXPECT_EQ(midi_interpret_vl_field(p_res_field).N,ans_n_bytes);
		EXPECT_EQ(midi_interpret_vl_field(p_res_field).val,e.num);
		for (int i=0; i<ans_n_bytes; ++i) {
			EXPECT_EQ(curr_result[i],e.ans[i]);
		}
	}
}

//
// Tests for:
// detect_chunk_type_result_t detect_chunk_type(const unsigned char*, int32_t=0);
// For each example, verifies that:
//   detect_chunk_type_result_t.size == detect_chunk_type_result_t.data_length + 8
//
TEST(midi_raw_tests, DetectChunkTypeTests) {
	struct tests {
		std::array<unsigned char,8> data {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
		uint32_t max_sz {0};
		chunk_type ans_type {chunk_type::invalid};
		uint32_t ans_data_length {0};
	};
	std::vector<tests> all_tests {
		// Invalid: no ID field
		{{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},100, chunk_type::invalid,0},
		// Invalid: MTrk, data_length > max_size
		{{0x4D,0x54,0x72,0x6B,0x00,0x00,0x00,0xFF},0x7F, chunk_type::invalid,255},
		// Invalid: MTrk, max_size<8
		{{0x4D,0x54,0x72,0x6B,0x00,0x00,0x00,0xFF},7, chunk_type::invalid,0},
		// Invalid: MTrk, data_length >= max_size
		{{0x4D,0x54,0x72,0x6B,0x00,0x00,0x00,0xFF},0xFF, chunk_type::invalid,255},
		// Invalid: MTrk, data_length==247 > max_size
		{{0x4D,0x54,0x72,0x6B,0x00,0x00,0x00,0xF7},0x7F, chunk_type::invalid,247},

		// valid, MThd, MTrk
		{{0x4D,0x54,0x68,0x64,0x00,0x00,0x00,0x00},100, chunk_type::header,0},
		{{0x4D,0x54,0x68,0x64,0x00,0x00,0x00,0x06},100, chunk_type::header,6},
		{{0x4D,0x54,0x72,0x6B,0x00,0x00,0x00,0x00},100, chunk_type::track,0},
		{{0x4D,0x54,0x72,0x6B,0x00,0x00,0x00,0x06},100, chunk_type::track,6},
		//{{0x4D,0x54,0x68,0x64,0x7F,0xFF,0xFF,0xFF}, chunk_type::header,2147483647},
		// Valid:  MTrk, data_length==247 => size == max_size
		{{0x4D,0x54,0x72,0x6B,0x00,0x00,0x00,0xF7},0xFF, chunk_type::track,247},

		// valid, unknown
		{{0x4D,0x54,0x4D,0x54,0x00,0x00,0x00,0x00},100, chunk_type::unknown,0},
		{{0x4D,0x54,0x68,0x65,0x00,0x00,0x00,0x06},100, chunk_type::unknown,6},
		{{0x4D,0x54,0x70,0x6B,0x00,0x00,0x00,0x00},100, chunk_type::unknown,0},
		{{0x4E,0x53,0x72,0x6B,0x00,0x00,0x00,0x06},100, chunk_type::unknown,6},

		// Invalid:  Size overflows an int32_t
		//{{0x4D,0x54,0x68,0x64,0xFF,0xFF,0xFF,0xFF},100, chunk_type::invalid,0},

		// Valid:  From random MIDI Files i have laying around
		{{0x4D,0x54,0x68,0x64,0x00,0x00,0x00,0x06},5584, chunk_type::header,6},
		{{0x4D,0x54,0x72,0x6B,0x00,0x00,0x00,0x1C},5584-14, chunk_type::track,28},
		{{0x4D,0x54,0x72,0x6B,0x00,0x00,0x0F,0x13},5584-50, chunk_type::track,3859},
		{{0x4D,0x54,0x72,0x6B,0x00,0x00,0x06,0x7B},5584-3917, chunk_type::track,1659},
	};
	auto x = 0x067B;
	for (const auto& e : all_tests) {
		auto res = detect_chunk_type(&(e.data[0]),e.max_sz);
		EXPECT_EQ(res.type,e.ans_type);
		if (res.type != chunk_type::invalid) {
			EXPECT_EQ(res.data_length,e.ans_data_length);
			EXPECT_EQ(res.size,e.ans_data_length+8);
		}
	}
}



//
// Tests for:
// enum class mthd_validation_error : uint8_t {
//     invalid_chunk, non_header_chunk,data_length_invalid,
//     inconsistent_ntrks_format_zero, unknown_error, no_error
// };
// struct validate_mthd_chunk_result_t {
//     const unsigned char *p {};  // points at the 'M' of "MThd"...
//     uint32_t size {0};  //  Always == reported size (data_length) + 8
//     mthd_validation_error error {mthd_validation_error::unknown_error};
//     bool is_valid {false};
// };
// validate_mthd_chunk_result_t validate_mthd_chunk(const unsigned char*, uint32_t=0);
//
TEST(midi_raw_tests, ValidateMThdChunkTests) {
	struct tests {
		std::array<unsigned char,14> data {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
		uint32_t max_sz {0};

		bool ans_is_valid {false};
		uint32_t ans_size {0};
		mthd_validation_error ans_error {mthd_validation_error::no_error};
	};
	std::vector<tests> all_tests {
		// Valid:  size==14<max_size
		{{0x4D,0x54,0x68,0x64,0x00,0x00,0x00,0x06,0x00,0x01,0x00,0x03,0x00,0xF0},100,
			true,14,mthd_validation_error::no_error},
		// Valid:  size==14-6+9<max_size
		{{0x4D,0x54,0x68,0x64,0x00,0x00,0x00,0x0F,0x00,0x01,0x00,0x03,0x00,0xF0},100,
			true,14-6+15,mthd_validation_error::no_error},
		// Valid:  size==14==max_size
		{{0x4D,0x54,0x68,0x64,0x00,0x00,0x00,0x06,0x00,0x01,0x00,0x03,0x00,0xF0},14,
			true,14,mthd_validation_error::no_error},

		// Valid:  Format==2
		{{0x4D,0x54,0x68,0x64,0x00,0x00,0x00,0x06,0x00,0x02,0x00,0x03,0x00,0xF0},100,
			true,14,mthd_validation_error::no_error},
		// Valid:  Format==9
		{{0x4D,0x54,0x68,0x64,0x00,0x00,0x00,0x06,0x00,0x09,0x00,0x03,0x00,0xF0},100,
			true,14,mthd_validation_error::no_error},
		// Valid:  ntrks==0
		{{0x4D,0x54,0x68,0x64,0x00,0x00,0x00,0x06,0x00,0x01,0x00,0x00,0x00,0xF0},100,
			true,14,mthd_validation_error::no_error},

		// Invalid: size==14 > max_size
		{{0x4D,0x54,0x68,0x64,0x00,0x00,0x00,0x06,0x00,0x01,0x00,0x03,0x00,0xF0},13,
			false,14,mthd_validation_error::invalid_chunk},
		{{0x4D,0x54,0x68,0x64,0x00,0x00,0x00,0x06,0x00,0x01,0x00,0x03,0x00,0xF0},0,
			false,14,mthd_validation_error::invalid_chunk},

		// Invalid: size==12>max_size, but the data_length field is < 6
		{{0x4D,0x54,0x68,0x64,0x00,0x00,0x00,0x04,0x00,0x01,0x00,0x03,0x00,0xF0},14,
			false,12,mthd_validation_error::data_length_invalid},

		// Invalid:  format==0 but ntrks==3
		{{0x4D,0x54,0x68,0x64,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x03,0x00,0xF0},100,
			false,14,mthd_validation_error::inconsistent_ntrks_format_zero},
	};

	for (const auto& e : all_tests) {
		auto res = validate_mthd_chunk(&(e.data[0]),e.max_sz);
		EXPECT_EQ(res.is_valid,e.ans_is_valid);
		EXPECT_EQ(res.error,e.ans_error);
		if (res.is_valid) {
			EXPECT_EQ(res.size,e.ans_size);
		}
	}
}

