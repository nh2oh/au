#include "gtest/gtest.h"
#include "..\aulib\scale\spn12tet.h"
#include "..\aulib\types\types_all.h"
#include <vector>


// Default constructor should generate A440
TEST(scale_spn12tet_tests, DefaultConstructor) {
	spn12tet sc {};
	EXPECT_TRUE(sc.n() == 12);
	EXPECT_TRUE(sc.to_frq(ntstr_t{"A(4)"}) == frq_t{440});

	std::vector<ntl_t> ntls_in_sc {"C"_ntl,"C#"_ntl,"D"_ntl,"D#"_ntl,"E"_ntl,
		"F"_ntl,"F#"_ntl,"G"_ntl,"G#"_ntl,"A"_ntl,"A#"_ntl,"B"_ntl};
	for (int i=0; i<sc.n(); ++i) {
		EXPECT_TRUE(sc.isinsc(ntls_in_sc[i]));
		EXPECT_TRUE(ntl_t{sc.to_ntstr(scd_t{i})} == ntls_in_sc[i]);
	}
	std::vector<ntl_t> ntls_not_in_sc {"S"_ntl,"S#"_ntl,"H"_ntl,"H#"_ntl,"J"_ntl,
		"CC"_ntl,"CC#"_ntl,"a"_ntl,"what#"_ntl,"y"_ntl,"a#"_ntl,"b"_ntl};
	for (const auto& e : ntls_not_in_sc) {
		EXPECT_FALSE(sc.isinsc(e));
	}
	
	// Conversions between scd_t, frq_t, ntstr_t
	// The range -1000->1000 is absurd but stresses the floating-point ==
	// operations for frq_t equality
	for (int i=-1000; i<1000; ++i) {
		scd_t ans_curr_scd {i};
		frq_t ans_curr_frq {sc.to_frq(ans_curr_scd)};
		rscdoctn_t curr_rscd {ans_curr_scd,12};
		octn_t ans_curr_oct {ans_curr_scd,12};
		ntl_t ans_curr_ntl {ntls_in_sc[curr_rscd.to_int()]};
		ntstr_t ans_curr_ntstr {ans_curr_ntl,ans_curr_oct};

		ntstr_t ntstr_from_scd {sc.to_ntstr(ans_curr_scd)};
		ntl_t ntl_from_scd {ntstr_from_scd};
		octn_t oct_from_ntstr_from_scd {ntstr_from_scd};
		octn_t oct_from_scd {sc.to_octn(ans_curr_scd)};
		EXPECT_TRUE(ntstr_from_scd == ans_curr_ntstr);
		EXPECT_TRUE(ntl_from_scd == ans_curr_ntl);
		EXPECT_TRUE(oct_from_ntstr_from_scd == ans_curr_oct);
		EXPECT_TRUE(oct_from_scd == ans_curr_oct);

		ntstr_t ntstr_from_frq {sc.to_ntstr(ans_curr_frq)};
		ntl_t ntl_from_frq {ntstr_from_frq};
		octn_t oct_from_frq {sc.to_octn(ans_curr_frq)};
		scd_t scd_from_frq {sc.to_scd(ans_curr_frq)};
		EXPECT_TRUE(ntstr_from_frq == ans_curr_ntstr);
		EXPECT_TRUE(ntl_from_frq == ans_curr_ntl);
		EXPECT_TRUE(oct_from_frq == ans_curr_oct);
		EXPECT_TRUE(scd_from_frq == ans_curr_scd);

		scd_t scd_from_ntstr {sc.to_scd(ans_curr_ntstr)};
		frq_t frq_from_ntstr {sc.to_frq(ans_curr_ntstr)};
		EXPECT_TRUE(scd_from_ntstr == ans_curr_scd);
		EXPECT_TRUE(frq_from_ntstr == ans_curr_frq);
	}
}

TEST(scale_spn12tet_tests, CSharpFour330Hz) {
	spn12tet::pitch_std ps {ntstr_t{"C#"_ntl,4},frq_t{330},2,12};
	spn12tet sc {ps};
	EXPECT_TRUE(sc.n() == 12);
	EXPECT_TRUE(sc.to_frq(ntstr_t{"C#(4)"}) == frq_t{330});

	std::vector<ntl_t> ntls_in_sc {"C"_ntl,"C#"_ntl,"D"_ntl,"D#"_ntl,"E"_ntl,
		"F"_ntl,"F#"_ntl,"G"_ntl,"G#"_ntl,"A"_ntl,"A#"_ntl,"B"_ntl};
	for (int i=0; i<sc.n(); ++i) {
		EXPECT_TRUE(sc.isinsc(ntls_in_sc[i]));
		EXPECT_TRUE(ntl_t{sc.to_ntstr(scd_t{i})} == ntls_in_sc[i]);
	}
	std::vector<ntl_t> ntls_not_in_sc {"S"_ntl,"S#"_ntl,"H"_ntl,"H#"_ntl,"J"_ntl,
		"CC"_ntl,"CC#"_ntl,"a"_ntl,"what#"_ntl,"y"_ntl,"a#"_ntl,"b"_ntl};
	for (const auto& e : ntls_not_in_sc) {
		EXPECT_FALSE(sc.isinsc(e));
	}
	
	// Conversions between scd_t, frq_t, ntstr_t
	// The range -1000->1000 is absurd but stresses the floating-point ==
	// operations for frq_t equality
	for (int i=-1000; i<1000; ++i) {
		scd_t ans_curr_scd {i};
		frq_t ans_curr_frq {sc.to_frq(ans_curr_scd)};
		rscdoctn_t curr_rscd {ans_curr_scd,12};
		octn_t ans_curr_oct {ans_curr_scd,12};
		ntl_t ans_curr_ntl {ntls_in_sc[curr_rscd.to_int()]};
		ntstr_t ans_curr_ntstr {ans_curr_ntl,ans_curr_oct};

		ntstr_t ntstr_from_scd {sc.to_ntstr(ans_curr_scd)};
		ntl_t ntl_from_scd {ntstr_from_scd};
		octn_t oct_from_ntstr_from_scd {ntstr_from_scd};
		octn_t oct_from_scd {sc.to_octn(ans_curr_scd)};
		EXPECT_TRUE(ntstr_from_scd == ans_curr_ntstr);
		EXPECT_TRUE(ntl_from_scd == ans_curr_ntl);
		EXPECT_TRUE(oct_from_ntstr_from_scd == ans_curr_oct);
		EXPECT_TRUE(oct_from_scd == ans_curr_oct);

		ntstr_t ntstr_from_frq {sc.to_ntstr(ans_curr_frq)};
		ntl_t ntl_from_frq {ntstr_from_frq};
		octn_t oct_from_frq {sc.to_octn(ans_curr_frq)};
		scd_t scd_from_frq {sc.to_scd(ans_curr_frq)};
		EXPECT_TRUE(ntstr_from_frq == ans_curr_ntstr);
		EXPECT_TRUE(ntl_from_frq == ans_curr_ntl);
		EXPECT_TRUE(oct_from_frq == ans_curr_oct);
		EXPECT_TRUE(scd_from_frq == ans_curr_scd);

		scd_t scd_from_ntstr {sc.to_scd(ans_curr_ntstr)};
		frq_t frq_from_ntstr {sc.to_frq(ans_curr_ntstr)};
		EXPECT_TRUE(scd_from_ntstr == ans_curr_scd);
		EXPECT_TRUE(frq_from_ntstr == ans_curr_frq);
	}
}


TEST(scale_spn12tet_tests, BThree551HzGint3ThirteenTET) {
	spn12tet::pitch_std ps {ntstr_t{"B"_ntl,3},frq_t{551},3,13};
	spn12tet sc {ps};
	EXPECT_TRUE(sc.n() == 12);
	EXPECT_TRUE(sc.to_frq(ntstr_t{"B(3)"}) == frq_t{551});

	std::vector<ntl_t> ntls_in_sc {"C"_ntl,"C#"_ntl,"D"_ntl,"D#"_ntl,"E"_ntl,
		"F"_ntl,"F#"_ntl,"G"_ntl,"G#"_ntl,"A"_ntl,"A#"_ntl,"B"_ntl};
	for (int i=0; i<sc.n(); ++i) {
		EXPECT_TRUE(sc.isinsc(ntls_in_sc[i]));
		EXPECT_TRUE(ntl_t{sc.to_ntstr(scd_t{i})} == ntls_in_sc[i]);
	}
	std::vector<ntl_t> ntls_not_in_sc {"S"_ntl,"S#"_ntl,"H"_ntl,"H#"_ntl,"J"_ntl,
		"CC"_ntl,"CC#"_ntl,"a"_ntl,"what#"_ntl,"y"_ntl,"a#"_ntl,"b"_ntl};
	for (const auto& e : ntls_not_in_sc) {
		EXPECT_FALSE(sc.isinsc(e));
	}
	
	// Conversions between scd_t, frq_t, ntstr_t
	// The range -1000->1000 is absurd but stresses the floating-point ==
	// operations for frq_t equality
	for (int i=-1000; i<1000; ++i) {
		scd_t ans_curr_scd {i};
		frq_t ans_curr_frq {sc.to_frq(ans_curr_scd)};
		rscdoctn_t curr_rscd {ans_curr_scd,12};
		octn_t ans_curr_oct {ans_curr_scd,12};
		ntl_t ans_curr_ntl {ntls_in_sc[curr_rscd.to_int()]};
		ntstr_t ans_curr_ntstr {ans_curr_ntl,ans_curr_oct};

		ntstr_t ntstr_from_scd {sc.to_ntstr(ans_curr_scd)};
		ntl_t ntl_from_scd {ntstr_from_scd};
		octn_t oct_from_ntstr_from_scd {ntstr_from_scd};
		octn_t oct_from_scd {sc.to_octn(ans_curr_scd)};
		EXPECT_TRUE(ntstr_from_scd == ans_curr_ntstr);
		EXPECT_TRUE(ntl_from_scd == ans_curr_ntl);
		EXPECT_TRUE(oct_from_ntstr_from_scd == ans_curr_oct);
		EXPECT_TRUE(oct_from_scd == ans_curr_oct);

		ntstr_t ntstr_from_frq {sc.to_ntstr(ans_curr_frq)};
		ntl_t ntl_from_frq {ntstr_from_frq};
		octn_t oct_from_frq {sc.to_octn(ans_curr_frq)};
		scd_t scd_from_frq {sc.to_scd(ans_curr_frq)};
		EXPECT_TRUE(ntstr_from_frq == ans_curr_ntstr);
		EXPECT_TRUE(ntl_from_frq == ans_curr_ntl);
		EXPECT_TRUE(oct_from_frq == ans_curr_oct);
		EXPECT_TRUE(scd_from_frq == ans_curr_scd);

		scd_t scd_from_ntstr {sc.to_scd(ans_curr_ntstr)};
		frq_t frq_from_ntstr {sc.to_frq(ans_curr_ntstr)};
		EXPECT_TRUE(scd_from_ntstr == ans_curr_scd);
		EXPECT_TRUE(frq_from_ntstr == ans_curr_frq);
	}
}

