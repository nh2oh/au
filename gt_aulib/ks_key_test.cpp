#include "gtest/gtest.h"
#include "..\aulib\melgen\randmel_gens.h"
#include "..\aulib\scale\diatonic_spn12tet.h"
#include "..\aulib\types\line_t.h"
#include "..\aulib\types\ntl_t.h"
#include "..\aulib\types\scd_t.h"
#include "..\aulib\types\ts_t.h"
#include "..\aulib\types\beat_bar_t.h"
#include "..\aulib\types\nv_t.h"

// TODO:  Need to fix scales before wasting time w/ more tests

TEST(ks_key_tests, CmajorScaleOnePassZeroOctave) {
	diatonic_spn12tet sc {ntl_t{"C"},diatonic_spn12tet::mode::major};
	// My "melody," nts_scds, nts_ntstrs is just C(0)-D(0)-...-B(0)
	// This should test positive for C-major
	std::vector<int> nts_scds {0,1,2,3,4,5,6};
	std::vector<ntstr_t> nts_ntstrs {};
	for (auto e : nts_scds) {
		nts_ntstrs.push_back(sc.to_ntstr(scd_t{e}));
	}
	rp_t rp {ts_t{4_bt,d::q},std::vector<d_t>(nts_ntstrs.size(),d::q)};
	line_t line {nts_ntstrs,rp};

	auto res = ks_key(line,ks_key_params{0});
	ntl_t ans_key {"C"};  // C
	bool ans_ismajor {true};
	EXPECT_TRUE(res.ismajor);
	EXPECT_TRUE(ans_key == res.key);
}

TEST(ks_key_tests, CminorScaleOnePassZeroOctave) {
	diatonic_spn12tet sc {ntl_t{"C"},diatonic_spn12tet::mode::minor};
	std::vector<int> nts_scds {0,1,2,3,4,5,6};
	std::vector<ntstr_t> nts_ntstrs {};
	for (auto e : nts_scds) {
		nts_ntstrs.push_back(sc.to_ntstr(scd_t{e}));
	}
	rp_t rp {ts_t{4_bt,d::q},std::vector<d_t>(nts_ntstrs.size(),d::q)};
	line_t line {nts_ntstrs,rp};

	auto res = ks_key(line,ks_key_params{0});
	// For some reason the ks alg assigns this sequence D#-major
	ntl_t ans_key {"D#"};
	EXPECT_TRUE(res.ismajor);
	EXPECT_TRUE(ans_key == res.key);
}

