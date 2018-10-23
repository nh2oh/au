#include "melgen\randmel_gens.h"
#include "types\line_t.h"
#include "types\ntl_t.h"
#include "types\scd_t.h"
#include "scale\spn12tet.h"
#include "util\au_random.h"
#include "util\au_algs.h"  // unique_n(rscds)
#include "util\au_algs_math.h"  // corr()
#include "dbklib\contigmap.h"
#include <array>
#include <algorithm>  // std::max_element()
//#include <iostream>
//#include "util\au_util.h"

//
// The Krumhansl-Schmuckler key-finding algorithm as described by p.52
// of Music and Probability (MIT Press) 2007 by David Temperly.  
// Also inspired by Temperley's C implementation (ks-key.c) obtained from
// http://davidtemperley.com/music-and-probability/
//

ks_key_result ks_key(line_t<ntstr_t> nts, ks_key_params p) {

	std::array<double,12> kp_base_maj;
	std::array<double,12> kp_base_min;
	switch (p.profile) {
		case 0:
			// From Temperly p.52
			//             C     C#    D     D#    E     F      F#    G     G#    A    A#    B
			kp_base_maj = {6.35, 2.23, 3.48, 2.33, 4.38, 4.09, 2.52, 5.19, 2.39, 3.66, 2.29, 2.88};
			kp_base_min = {6.33, 2.68, 3.52, 5.38, 2.60, 3.53, 2.54, 4.75, 3.98, 2.69, 3.34, 3.17};
			break;
		case 1:
			// Page 180 of "The Cognition of Basic Musical Structures" by Temperley
			//             C  C# D    D# E    F  F# G    G# A    A#   B
			kp_base_maj = {5, 2, 3.5, 2, 4.5, 4, 2, 4.5, 2, 3.5, 1.5, 4};
			kp_base_min = {5, 2, 3.5, 4.5, 2, 4, 2, 4.5, 3.5, 2, 1.5, 4};
			break;
	}

	// kp_maj[key][rscd], kp_min[][]
	// For key i=0, element 0 of the base profile needs to appear in row 0.  
	// For key i=1, element _0_ of the base profile needs to appear in row 
	// 1, ...
	std::vector<std::vector<double>> kp_maj(12,std::vector<double>(12,0.0));
	std::vector<std::vector<double>> kp_min(12,std::vector<double>(12,0.0));
	for (int i=0; i<12; ++i) {
		for (int j=0; j<12; ++j) {
			kp_maj[i][j] = kp_base_maj[(j-i+12)%12];
			kp_min[i][j] = kp_base_min[(j-i+12)%12];
		}
	}

	// The note-sequence / rscd weights
	// Each rscd is weighted according to the total amount of time it occupies 
	// in the sequence.  Chords are flattened (TODO:  Not sure how Temperley
	// deals with chords).  
	// rscds_weighted[0] => total duration for 'C', rscds_weighted[1] => C#, ...
	// Relies on the fact that for spn12tet, C => rscd == 0, etc.  
	spn12tet sc {};
	auto notes_flatchords_norests = nts.notes_flat();

	dbk::contigmap<rscdoctn_t,double> rw {};
	std::vector<double> rscds_weighted(12,0.0);
	d_t dw {d::w};
	for (size_t i=0; i<notes_flatchords_norests.size(); ++i) {
		int curr_rscd = rscdoctn_t{sc.to_scd(notes_flatchords_norests[i].note),12}.to_int();
		rscds_weighted[curr_rscd] += (notes_flatchords_norests[i].d)/dw;

		rscdoctn_t cr {sc.to_scd(notes_flatchords_norests[i].note),12};
		rw[cr] += (notes_flatchords_norests[i].d)/dw;
	}

	auto corr_maj = corr(rscds_weighted,kp_maj);
	auto corr_min = corr(rscds_weighted,kp_min);
	auto rscd_max_corr_maj = std::distance(
		corr_maj.begin(),
		std::max_element(corr_maj.begin(), corr_maj.end()));
	auto rscd_max_corr_min = std::distance(
		corr_min.begin(),
		std::max_element(corr_min.begin(), corr_min.end()));

	//std::cout << printm(kp_maj,"%10.3f") << std::endl << std::endl;
	//std::cout << printm(kp_min,"%10.3f") << std::endl << std::endl;

	ks_key_result res {};
	if (corr_maj[rscd_max_corr_maj] >= corr_min[rscd_max_corr_min]) {
		// NOTE:  >= => report major if there is a tie
		res.key = ntl_t{sc.to_ntstr(scd_t{static_cast<int>(rscd_max_corr_maj)})};
		res.ismajor = true;
		res.score = corr_maj[rscd_max_corr_maj];
	} else {
		res.key = ntl_t{sc.to_ntstr(scd_t{static_cast<int>(rscd_max_corr_min)})};
		res.ismajor = false;
		res.score = corr_maj[rscd_max_corr_min];
	}

	for (int i=0; i<12; ++i) {
		res.all_scores[0][i] = corr_maj[i];
		res.all_scores[1][i] = corr_min[i];
	}

	return res;
}


