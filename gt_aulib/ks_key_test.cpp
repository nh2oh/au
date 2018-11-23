#include "gtest/gtest.h"
#include "..\aulib\melgen\randmel_gens.h"
#include "..\aulib\scale\diatonic_spn.h"
#include "..\aulib\scale\spn.h"
#include "..\aulib\types\line_t.h"
#include "..\aulib\types\ntl_t.h"
#include "..\aulib\types\scd_t.h"
#include "..\aulib\types\ts_t.h"
#include "..\aulib\types\beat_bar_t.h"
#include "..\aulib\types\nv_t.h"
#include <numeric>  // std::round()

TEST(ks_key_tests, CmajorScaleOnePassZeroOctave) {
	spn sc {};
	std::vector<ntl_t> cmaj_ntls {"C"_ntl,"D"_ntl,"E"_ntl,"F"_ntl,"G"_ntl,"A"_ntl,"B"_ntl};

	std::vector<note_t> melody_notes {};
	for (const auto& e : cmaj_ntls) {
		melody_notes.push_back(sc[sc.to_scd(e,octn_t{0})]);
	}
	rp_t melody_rp {ts_t{4_bt,d::q},std::vector<d_t>(melody_notes.size(),d::q)};
	line_t melody {melody_notes,melody_rp};

	auto res = ks_key(melody,ks_key_params{0});

	ntl_t ans_key {"C"};  // C
	bool ans_ismajor {true};
	EXPECT_TRUE(res.ismajor);
	EXPECT_TRUE(ans_key == res.key);
}


TEST(ks_key_tests, CminorScaleOnePassZeroOctave) {
	// Comparison to Temperley's C implementation on the following notes file:
	// Note      0    200 60
	// Note    200    400 62
	// Note    400    600 63
	// Note    600    800 65
	// Note   1000   1200 67
	// Note   1200   1400 68
	// Note   1400   1600 70
	std::vector<ntl_t> cmin_ntls {"C"_ntl,"D"_ntl,"D#"_ntl,"F"_ntl,"G"_ntl,"G#"_ntl,"A#"_ntl};
	spn sc {};

	std::vector<note_t> melody_notes {};
	for (const auto& e : cmin_ntls) {
		melody_notes.push_back(sc[sc.to_scd(e,octn_t{0})]);
	}
	rp_t melody_rp {ts_t{4_bt,d::q},std::vector<d_t>(melody_notes.size(),d::q)};
	line_t melody {melody_notes,melody_rp};

	auto res = ks_key(melody,ks_key_params{0});

	std::vector<double> res_maj_scores {};
	std::vector<double> res_min_scores {};
	for (const auto& e : res.all_scores) {
		res_maj_scores.push_back(e.v.maj);
		res_min_scores.push_back(e.v.min);
	}
	// For some reason the ks alg assigns this sequence D#-major
	ntl_t ans_key {"D#"};
	std::vector<double> ans_maj_scores {0.23318,-0.00502,-0.29139,0.75641,-0.71693,
		0.40313,-0.37971,0.06591,0.54631,-0.75574,0.67745,-0.53360};
	std::vector<double> ans_min_scores {0.71213,-0.58748,0.26575,0.09159,-0.33283,
		0.52333,-0.70018,0.58919,-0.05330,-0.19672,0.11647,-0.42796};
	EXPECT_TRUE(res.ismajor);
	EXPECT_TRUE(ans_key == res.key);

	for (int i=0; i<12; ++i) {
		EXPECT_EQ(std::round(1000*ans_maj_scores[i]),
			std::round(1000*res_maj_scores[i]));
		EXPECT_EQ(std::round(1000*ans_min_scores[i]),
			std::round(1000*res_min_scores[i]));
	}
}


TEST(ks_key_tests, ChildO5Notes) {
	spn sc {};
	std::vector<ntl_t> cmaj_ntls {"C"_ntl,"D"_ntl,"E"_ntl,"F"_ntl,"G"_ntl,"A"_ntl,"B"_ntl};
	ts_t ts {4_bt,d::q};

	std::vector<int> midi_code {71,69,67,67,67,71,72,74,76,74,71,67,71,69,67,67,72,
		71,69,67,67,71,72,74,76,74,71,67,71,69,67,67};
	std::vector<double> ontime {0,288,620,1227,1500,1789,2074,2418,3669,3969,4312,
		4935,5096,5546,6046,6224,7199,7502,7797,8122,8760,9360,9651,9987,11252,
		11549,11893,12531,12695,13176,13693,13897};
	std::vector<double> offtime {299,607,1086,1343,1773,2087,2436,3471,3952,4330,
		4913,5065,5557,6023,6131,6995,7499,7811,8124,8636,9371,9678,10013,11024,
		11552,11908,12475,12673,13195,13673,13783,15133};
	
	// Crude quantization of the time intervals.  As of this writing i have no function
	// that just does this.  
	// Also convert the midi notes to scds.  
	std::vector<d_t> nvs {};
	std::vector<note_t> melody_notes {};
	for (int i=0; i<ontime.size(); ++i) {
		//double num_64th_notes = std::round((offtime[i]-ontime[i])/dt_ms_64th_note);
		//d_t curr_nv = nv_sf*num_64th_notes;
		d_t curr_nv {(offtime[i]-ontime[i])/1000.0};
		nvs.push_back(curr_nv);

		melody_notes.push_back(sc[midi_code[i]]);
	}
	rp_t melody_rp {ts,nvs};
	line_t<note_t> melody {melody_notes,melody_rp};

	auto res = ks_key(melody,ks_key_params{0});

	ntl_t ans_key {"G"};
	EXPECT_TRUE(res.ismajor);
	EXPECT_TRUE(ans_key == res.key);
}


