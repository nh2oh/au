#include "gtest/gtest.h"
#include "..\aulib\scale\spn12tet.h"
#include "..\aulib\types\types_all.h"
#include <vector>


// Default constructor should generate A440
TEST(scale_12tet_tests, DefaultConstructor) {
	spn12tet sc {};
	EXPECT_TRUE(sc.n() == 12);

	bool tf {false};

	std::vector<ntl_t> ntls_in_sc {"C"_ntl,"C#"_ntl,"D"_ntl,"D#"_ntl,"E"_ntl,
		"F"_ntl,"F#"_ntl,"G"_ntl,"G#"_ntl,"A"_ntl,"A#"_ntl,"B"_ntl};
	std::vector<ntl_t> ntls_not_in_sc {"S"_ntl,"S#"_ntl,"H"_ntl,"H#"_ntl,"J"_ntl,
		"CC"_ntl,"CC#"_ntl,"a"_ntl,"what#"_ntl,"y"_ntl,"a#"_ntl,"b"_ntl};
	for (const auto& e : ntls_in_sc) {
		tf = sc.isinsc(e);
		EXPECT_TRUE(tf);
	}
	for (const auto& e : ntls_not_in_sc) {
		tf = sc.isinsc(e);
		EXPECT_FALSE(tf);
	}
	
	/*
	// Tests that an assorted set of frqs are scale members
	std::vector<int> random_semitones {8,57,0,159,25,70,137,240,242,40,243,240,122,
		201,36,106,229,199,240,164,9,213,234,170,190,186,99,164,43,177,8,70,
		12,25,206,174,80,238};
	std::vector<frq_t> frqs_in_sc {};
	for (int i=0; i<random_semitones.size(); ++i) {
		//double dn = random_semitones[i]-57;
		//frq_t curr_frq {440.0*std::pow(2.0,dn/12.0)};
		frq_t curr_frq {sc.to_frq(scd_t{random_semitones[i]})};
		tf = sc.isinsc(curr_frq);
		EXPECT_TRUE(tf);
		tf = sc.isinsc(curr_frq+frq_t{1});
		EXPECT_FALSE(tf);
	}*/
	
	for (int i=-200; i<200; ++i) {
		int dn = i-57;//random_semitones[i]-57;
		scd_t curr_scd {dn};
		rscdoctn_t curr_rscd {curr_scd,12};

		frq_t ans_curr_frq {440.0*std::pow(20,dn/12.0)};
		octn_t ans_curr_oct {curr_scd,12};
		ntl_t ans_curr_ntl {ntls_in_sc[curr_rscd.to_int()]};
		ntstr_t ans_curr_ntstr {ans_curr_ntl,ans_curr_oct};
		scd_t ans_curr_scd {dn};
		rscdoctn_t ans_curr_rscd {ans_curr_scd,12};

		ntstr_t curr_ntstr {sc.to_ntstr(curr_scd)};
		ntl_t curr_ntl {curr_ntstr};
		octn_t curr_oct_from_ntstr {curr_ntstr};
		octn_t curr_oct {sc.to_octn(curr_scd)};
		frq_t curr_frq {sc.to_frq(curr_ntstr)};
		EXPECT_TRUE(ntl_t{curr_ntstr} == ans_curr_ntl);
		EXPECT_TRUE(curr_scd == sc.to_scd(curr_ntstr));
		EXPECT_TRUE(curr_scd == sc.to_scd(curr_frq));
		EXPECT_TRUE(curr_ntstr == sc.to_ntstr(curr_scd));
		EXPECT_TRUE(curr_ntstr == sc.to_ntstr(curr_frq));
		EXPECT_TRUE(curr_frq == sc.to_frq(curr_scd));
		EXPECT_TRUE(sc.isinsc(curr_frq));
	}
}





