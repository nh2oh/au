#include "gtest/gtest.h"
#include "..\aulib\scale\scale_diatonic12tet.h"
#include "..\aulib\types\types_all.h"
#include <vector>


// Default constructor should generate A440
TEST(scale_diatonic12tet_tests, DefaultConstructor) {
	frq_t ans_ref_frq {440};
	int ans_gint {2};
	int ans_int_per_oct {12};
	std::vector<int> ans_idx_ntl {6,0,1,2,3,4,5};
	scale_diatonic12tet sc {};
	EXPECT_TRUE(sc.n() == 7);
	std::vector<ntl_t> ntls_in_sc {"C#"_ntl,"D"_ntl,"E"_ntl,
		"F"_ntl,"F#"_ntl,"G#"_ntl,"A"_ntl};
	std::vector<ntl_t> ntls_not_in_sc {"S"_ntl,"S#"_ntl,"H"_ntl,"H#"_ntl,"J"_ntl,
		"CC"_ntl,"CC#"_ntl,"a"_ntl,"what#"_ntl,"y"_ntl,"a#"_ntl,"b"_ntl,"B"_ntl,
		"C"_ntl,"G"_ntl,"D#"_ntl};
	for (const auto& e : ntls_in_sc) {
		EXPECT_TRUE(sc.isinsc(e));
	}
	for (const auto& e : ntls_not_in_sc) {
		EXPECT_FALSE(sc.isinsc(e));
	}
	
	// Tests that an assorted set of frqs are scale members
	std::vector<int> random_semitones {0,159,25,70,137,240,242,40,243,240,122,
		201,36,106,229,199,240,164,9,213,234,170,190,186,99,164,43,177,8,70,
		12,25,206,174,80,238};
	std::vector<frq_t> frqs_in_sc {};
	for (int i=0; i<random_semitones.size(); ++i) {
		frq_t curr_frq = frq_eqt(random_semitones[i], 
			ans_ref_frq, ans_int_per_oct, ans_gint);
		EXPECT_TRUE(sc.isinsc(curr_frq));
		EXPECT_FALSE(sc.isinsc(curr_frq+frq_t{1}));
	}
	
	for (int i=0; i<random_semitones.size(); ++i) {
		ntl_t ans_curr_ntl {ntls_in_sc[ans_idx_ntl[random_semitones[i]%7]]};
		scd_t curr_scd {random_semitones[i]};
		rscdoctn_t curr_rscd {curr_scd,12};
		ntstr_t curr_ntstr {sc.to_ntstr(curr_scd)};
		frq_t curr_frq {sc.to_frq(curr_ntstr)};
		//EXPECT_TRUE(ntl_t{curr_ntstr} == ans_curr_ntl);
		//EXPECT_TRUE(curr_scd == sc.to_scd(curr_ntstr));
		//EXPECT_TRUE(curr_scd == sc.to_scd(curr_frq));
		//EXPECT_TRUE(curr_ntstr == sc.to_ntstr(curr_scd));
		//EXPECT_TRUE(curr_ntstr == sc.to_ntstr(curr_frq));
		//EXPECT_TRUE(curr_frq == sc.to_frq(curr_scd));
	}
}



