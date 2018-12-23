#include "melgen\randmel_gens.h"
#include "types\line_t.h"
#include "types\ntl_t.h"
#include "util\au_random.h"  // corr()
#include "dbklib\contigumap.h"
#include "dbklib\stats.h"

//
// The Krumhansl-Schmuckler key-finding algorithm as described by p.52
// of Music and Probability (MIT Press) 2007 by David Temperly.  
// Also inspired by Temperley's C implementation (ks-key.c) obtained from
// http://davidtemperley.com/music-and-probability/
//
//
ks_key_result ks_key(line_t<note_t> nts, ks_key_params p) {
	const std::vector<ntl_t> cchrom_ntls {"C"_ntl,"C#"_ntl,"D"_ntl,"D#"_ntl,
		"E"_ntl,"F"_ntl,"F#"_ntl,"G"_ntl,"G#"_ntl,"A"_ntl,"A#"_ntl,"B"_ntl};

	std::vector<double> values_basekp_maj;
	std::vector<double> values_basekp_min;
	switch (p.profile) {
		case 0:
			// From Temperly p.52
			//                   C    C#   D    D#   E    F    F#   G    G#   A    A#   B
			values_basekp_maj = {6.35,2.23,3.48,2.33,4.38,4.09,2.52,5.19,2.39,3.66,2.29,2.88};
			values_basekp_min = {6.33,2.68,3.52,5.38,2.60,3.53,2.54,4.75,3.98,2.69,3.34,3.17};
			break;
		case 1:
			// Page 180 of "The Cognition of Basic Musical Structures" by Temperley
			//                   C  C# D    D# E    F  F# G    G# A    A#   B
			values_basekp_maj = {5, 2, 3.5, 2, 4.5, 4, 2, 4.5, 2, 3.5, 1.5, 4};
			values_basekp_min = {5, 2, 3.5, 4.5, 2, 4, 2, 4.5, 3.5, 2, 1.5, 4};
			break;
	}
	struct kp_major_minor {
		std::vector<double> maj {};
		std::vector<double> min {};
	};
	dbk::contigumap<ntl_t,kp_major_minor> kps {};
	for (int i=0; i<12; ++i) {
		std::vector<double> curr_kpvals_maj {};  curr_kpvals_maj.reserve(12);
		std::vector<double> curr_kpvals_min {};  curr_kpvals_min.reserve(12);
		for (int j=0; j<12; ++j) {
			// For key i=0, element 0 of the base profile needs to appear in row 0.  
			// For key i=1, element _0_ of the base profile needs to appear in row 
			// 1, ...
			curr_kpvals_maj.push_back(values_basekp_maj[(j-i+12)%12]);
			curr_kpvals_min.push_back(values_basekp_min[(j-i+12)%12]);
		}
		kps[cchrom_ntls[i]] = {curr_kpvals_maj,curr_kpvals_min};
	}

	// 
	// Each ntl is weighted according to the total amount of time it occupies 
	// in the sequence.  Chords are flattened (TODO:  Not sure how Temperley
	// deals with chords).  
	auto melody_flat = nts.notes_flat();
	dbk::contigumap<ntl_t,double> note_wtcount {cchrom_ntls,0.0};  // "note weighted-count"
	d_t dw {d::w};
	for (const auto& e : melody_flat) {
		note_wtcount[e.note.ntl] += (e.d)/dw;
	}


	ks_key_result res {};
	for (const auto& curr_ntl : cchrom_ntls) { 
		res.all_scores[curr_ntl] = {
			corr(note_wtcount.values(),kps[curr_ntl].maj),
			corr(note_wtcount.values(),kps[curr_ntl].min)
		};

		if (res.all_scores[curr_ntl].maj >= res.score) {
			res.score = res.all_scores[curr_ntl].maj;
			res.ismajor = true;
			res.key = curr_ntl;
		} else if (res.all_scores[curr_ntl].min > res.score) {
			res.score = res.all_scores[curr_ntl].min;
			res.ismajor = false;
			res.key = curr_ntl;
		}
	}

	return res;
}

