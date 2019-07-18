#include "gtest/gtest.h"
#include "..\aulib\input\midi\mthd_t.h"
#include "mthd_test_data.h"
#include <array>
#include <cstdint>

// TODO:  Fix the acrobatics about making the time_division_t objects...

//
// Tests for:
// mthd_t();
//
// The default ctor should create a format 1 file w/ 0 trks and
// a division field => 120 tpq
//
TEST(mthd_tests, defaultCtor) {
	std::array<unsigned char,14> ans {
		0x4Du,0x54u,0x68u,0x64u, 0x00u,0x00u,0x00u,0x06u,
		0x00u,0x01u, 0x00u,0x00u, 0x00u,0x78u
	};
	auto mthd = mthd_t();
	EXPECT_EQ(mthd.size(), ans.size());
	EXPECT_EQ(mthd.nbytes(), ans.size());
	
	auto cit=mthd.cbegin();
	auto cend=mthd.cend();
	for (int i=0; i<mthd.size(); ++i) {
		EXPECT_NE(cit,cend);
		EXPECT_EQ(*cit++,ans[i]);
	}
	EXPECT_EQ(cit,cend);

	EXPECT_EQ(mthd.length(),6);
	time_division_t tdf_ans = make_time_division_from_raw(0x0078u).value;
	auto tdf_ans_rv = tdf_ans.get_raw_value();
	EXPECT_EQ(mthd.division().get_raw_value(),tdf_ans.get_raw_value());
	EXPECT_EQ(mthd.division().get_tpq(),static_cast<uint16_t>(120));
	EXPECT_EQ(mthd.format(),1);
	EXPECT_EQ(mthd.ntrks(),0);
}

//
// Tests for:
// explicit mthd_t(int32_t fmt, int32_t ntrks, time_division_t div);
//
// With valid input combinations.  
//
TEST(mthd_tests, fieldValueCtorWithValidValues) {
	for (const auto& tcase : mthd_test::byfieldvalue_valid) {
		mthd_t mthd = mthd_t(tcase.format, tcase.ntrks, tcase.division);
		EXPECT_EQ(mthd.size(), 14);
		EXPECT_EQ(mthd.nbytes(), 14);
		EXPECT_EQ(mthd.length(), 6);
		EXPECT_EQ(mthd.format(), tcase.format);
		EXPECT_EQ(mthd.ntrks(), tcase.ntrks);
		auto d = mthd.division();
		EXPECT_TRUE(is_tpq(d));
		EXPECT_FALSE(is_smpte(d));
		EXPECT_EQ(d.get_tpq(), tcase.division);
	}
}

//
// Tests for:
// explicit mthd_t(int32_t fmt, int32_t ntrks, time_division_t div);
//
// With _invalid_ input combinations.  
//
TEST(mthd_tests, fieldValueCtorWithInvalidValues) {
	for (const auto& tcase : mthd_test::byfieldvalue_invalid) {
		mthd_t mthd = mthd_t(tcase.format, tcase.ntrks, tcase.division);
		EXPECT_EQ(mthd.size(), 14);
		EXPECT_EQ(mthd.nbytes(), 14);
		EXPECT_EQ(mthd.length(), 6);
		EXPECT_EQ(mthd.format(), tcase.ans_format);
		EXPECT_EQ(mthd.ntrks(), tcase.ans_ntrks);
		auto d = mthd.division();
		EXPECT_TRUE(is_tpq(d));
		EXPECT_FALSE(is_smpte(d));
		EXPECT_EQ(d.get_tpq(), tcase.ans_division);
	}
}

//
// Tests for:
// maybe_mthd_t make_mthd(const unsigned char *end, const unsigned char *end);
//
// With invalid input data.  
//
TEST(mthd_tests, MakeMthdInvalidInput) {
	for (const auto& tcase : mthd_test::invalid_set_a) {
		auto beg = tcase.data.data();
		auto end = beg + tcase.offset_to_data_end;
		auto mthd = make_mthd(beg,end);
		
		EXPECT_FALSE(mthd);
	}
}


//
// Tests for:
// maybe_mthd_t make_mthd(const unsigned char *end, const unsigned char *end);
//
// With _valid_ input data
//
TEST(mthd_tests, MakeMthdValidInput) {
	for (const auto& tcase : mthd_test::valid_set_a) {
		auto beg = tcase.data.data();
		auto end = beg + tcase.offset_to_data_end;
		auto mthd = make_mthd(beg,end);
		EXPECT_TRUE(mthd);
	}
}


//
// Tests for:
// maybe_mthd_t make_mthd(const unsigned char *end, const unsigned char *end);
//
// With valid, but "unusual" input data observed in the wild
//
TEST(mthd_tests, MakeMthdValidUnusualInput) {
	for (const auto& tcase : mthd_test::valid_unusual_a) {
		auto beg = tcase.data.data();
		auto end = tcase.data.data() + tcase.data.size();
		auto mthd = make_mthd(beg,end);
		EXPECT_TRUE(mthd);
		EXPECT_EQ(mthd.mthd.length(),tcase.ans_length);
		EXPECT_EQ(mthd.mthd.format(),tcase.ans_format);
		EXPECT_EQ(mthd.mthd.ntrks(),tcase.ans_ntrks);
		EXPECT_EQ(mthd.mthd.division().get_tpq(),tcase.ans_division);
	}
}

TEST(mthd_tests, interpretSMPTEField) {
	struct test_t {
		uint16_t input {0};
		int8_t ans_tcf {0};
		uint8_t ans_upf {0};
	};
	std::array<test_t,3> tests {
		test_t {0xE250u,-30,80},  // p.133 of the midi std
		test_t {0xE728u,-25,40},   // 25fr/sec * 40tk/fr => 1000tk/sec => ms resolution
		test_t {0xE350u,-29,80}
	};

	for (const auto& e : tests) {
		auto curr_maybe = make_time_division_from_raw(e.input);
		time_division_t curr_tdf = curr_maybe.value;
		EXPECT_EQ(curr_tdf.get_type(),time_division_t::type::smpte);

		auto curr_tcf = curr_tdf.get_smpte().time_code; //get_time_code_fmt(curr_tdf);
		EXPECT_EQ(curr_tcf,e.ans_tcf);
		auto curr_upf = curr_tdf.get_smpte().subframes; //get_units_per_frame(curr_tdf);
		EXPECT_EQ(curr_upf,e.ans_upf);
	}
}

