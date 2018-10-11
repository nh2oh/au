#include "melgen/randmel_gens.h"
#include "types/line_t.h"
#include "types\ntl_t.h"
#include "scale\scale_12tet.h"
#include "util\au_random.h"
#include "util\au_algs.h"  // unique_n(rscds)
#include "util\au_algs_math.h"
#include <array>
#include <algorithm>  // std::max_element()



kk_key_result kk_key(line_t<ntstr_t> nts, kk_key_params p) {

	std::array<double,12> kp_base_maj;
	std::array<double,12> kp_base_min;
	switch (p.profile) {
		case 0:
			// Algorithm of Krumhansl & Kessler as described by Temperly p.52
			//        C     C#    D     D#    E     F      F#    G     G#    A    A#    B
			kp_base_maj = {6.35, 2.23, 3.48, 2.33, 4.38, 4.09, 2.52, 5.19, 2.39, 3.66, 2.29, 2.88};
			kp_base_min = {6.33, 2.68, 3.52, 5.38, 2.60, 3.53, 2.54, 4.75, 3.98, 2.69, 3.34, 3.17};
			break;
		case 1:
			// Page 180 of "The Cognition of Basic Musical Structures" by Temperley
			//        C  C# D    D# E    F  F# G    G# A    A#   B
			kp_base_maj = {5, 2, 3.5, 2, 4.5, 4, 2, 4.5, 2, 3.5, 1.5, 4};
			kp_base_min = {5, 2, 3.5, 4.5, 2, 4, 2, 4.5, 3.5, 2, 1.5, 4};
			break;
	}
	std::vector<std::vector<double>> kp_maj(12,std::vector<double>(12,0.0));
	std::vector<std::vector<double>> kp_min(12,std::vector<double>(12,0.0));
	for (int i=0; i<kp_base_maj.size(); ++i) {
		for (int j=0; j<kp_base_maj.size(); ++j) {
			kp_maj[i][j] = kp_base_maj[(i+j)%12];
			kp_min[i][j] = kp_base_min[(i+j)%12];
		}
	}

	scale_12tet sc {};
	auto notes_flatchords_norests = nts.notes();
	std::vector<rscdoctn_t> rscds(notes_flatchords_norests.size(),rscdoctn_t{scd_t{0},12});
	for (int i=0; i<notes_flatchords_norests.size(); ++i) {
		auto curr_rscd = rscdoctn_t{*(sc.to_scd(notes_flatchords_norests[i])),12};
		rscds[i] = curr_rscd;
	}
	auto uqn_rscds = unique_n(rscds);
	std::vector<double> rscd_counts(12,0.0);
	for (int i=0; i<rscd_counts.size(); ++i) {
		rscd_counts[i] = uqn_rscds[rscdoctn_t{scd_t{i},12}];
	}

	auto corr_maj = corr(rscd_counts,kp_maj);
	auto corr_min = corr(rscd_counts,kp_min);
	auto rscd_max_corr_maj = std::distance(
		corr_maj.begin(),
		std::max_element(corr_maj.begin(), corr_maj.end()));
	auto rscd_max_corr_min = std::distance(
		corr_min.begin(),
		std::max_element(corr_min.begin(), corr_min.end()));


	kk_key_result res {};
	if (corr_maj[rscd_max_corr_maj] >= corr_min[rscd_max_corr_min]) {
		// NOTE:  >= => report major if there is a tie
		res.key = scd_t{static_cast<int>(rscd_max_corr_maj)};
		res.ismajor = true;
		res.score = corr_maj[rscd_max_corr_maj];
	} else {
		res.key = scd_t{static_cast<int>(rscd_max_corr_min)};
		res.ismajor = false;
		res.score = corr_maj[rscd_max_corr_min];
	}

	for (int i=0; i<corr_maj.size(); ++i) {
		res.all_scores[0][i] = corr_maj[i];
	}
	for (int i=0; i<corr_min.size(); ++i) {
		res.all_scores[1][i] = corr_min[i];
	}

	return res;

}


