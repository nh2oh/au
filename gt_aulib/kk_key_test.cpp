#include "gtest/gtest.h"
#include "..\aulib\melgen\randmel_gens.h"
#include "..\aulib\scale\scale_12tet.h"
#include "..\aulib\scale\diatonic_spn12tet.h"
#include "..\aulib\types\line_t.h"
#include "..\aulib\types\ntl_t.h"
#include "..\aulib\types\scd_t.h"
#include "..\aulib\types\ts_t.h"
#include "..\aulib\types\beat_bar_t.h"
#include "..\aulib\types\nv_t.h"

// TODO:  Need to fix scales before wasting time w/ more tests

TEST(kk_key_tests, yay) {
	diatonic_spn12tet sc {ntl_t{"C"},diatonic_spn12tet::mode::major};
	std::vector<scd_t> nts_scds {scd_t{0},scd_t{1},scd_t{2},scd_t{3},scd_t{4},scd_t{5},scd_t{6}};
	std::vector<ntstr_t> nts_ntstrs {};
	for (auto e : nts_scds) {
		nts_ntstrs.push_back(sc.to_ntstr(e));
	}
	rp_t rp {ts_t{4_bt,d::q},std::vector<d_t>(nts_ntstrs.size(),d::q)};
	line_t line {nts_ntstrs,rp};

	auto res = kk_key(line,kk_key_params{0});
	scd_t ans_key_scd {0};  // C
	bool ans_ismajor {true};
	EXPECT_TRUE(res.ismajor);
	EXPECT_TRUE(ans_key_scd == res.key);
}

