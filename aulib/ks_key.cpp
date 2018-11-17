#include "melgen\randmel_gens.h"
#include "types\line_t.h"
#include "types\ntl_t.h"
#include "types\scd_t.h"
#include "scale\spn12tet.h"  // TODO:  Drop
#include "scale\spn12tet3.h"
#include "util\au_random.h"
#include "util\au_algs.h"  // unique_n(rscds)
#include "util\au_algs_math.h"  // corr()
#include "dbklib\contigumap.h"
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

ks_key_result ks_key(line_t<note_t> nts, ks_key_params p) {
	spn12tet sc {};
	spn12tet3 sc3 {};

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
	dbk::contigumap<ntl_t,std::vector<double>> kp_maj {};
	dbk::contigumap<ntl_t,std::vector<double>> kp_min {};
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
		kp_maj[cchrom_ntls[i]] = curr_kpvals_maj;
		kp_min[cchrom_ntls[i]] = curr_kpvals_min;
	}

	/*
	for (const auto& curr_ntl : cchrom_ntls) {
		for (int j=0; j<12; ++j) {
			int i = kp_maj.begin()
			kp_maj[curr_ntl][j] = kp_base_maj[(j-i+12)%12];
			kp_min[curr_ntl][j] = kp_base_min[(j-i+12)%12];
		}
		//mkp_maj[sc.to_ntstr(scd_t{i})] = kp_maj[i];
		//mkp_min[sc.to_ntstr(scd_t{i})] = kp_min[i];
		auto curr_note = *(sc3.to_scd(i));
		mkp_maj[curr_note.ntl] = kp_maj[i];
		mkp_min[curr_note.ntl] = kp_min[i];
	}*/



	/*
	// kp_maj[key][rscd], kp_min[][]
	// For key i=0, element 0 of the base profile needs to appear in row 0.  
	// For key i=1, element _0_ of the base profile needs to appear in row 
	// 1, ...
	dbk::contigumap<ntl_t,std::vector<double>> mkp_maj {};
	dbk::contigumap<ntl_t,std::vector<double>> mkp_min {};

	std::vector<std::vector<double>> kp_maj(12,std::vector<double>(12,0.0));
	std::vector<std::vector<double>> kp_min(12,std::vector<double>(12,0.0));
	for (int i=0; i<12; ++i) {
		for (int j=0; j<12; ++j) {
			kp_maj[i][j] = kp_base_maj[(j-i+12)%12];
			kp_min[i][j] = kp_base_min[(j-i+12)%12];
		}
		//mkp_maj[sc.to_ntstr(scd_t{i})] = kp_maj[i];
		//mkp_min[sc.to_ntstr(scd_t{i})] = kp_min[i];
		auto curr_note = *(sc3.to_scd(i));
		mkp_maj[curr_note.ntl] = kp_maj[i];
		mkp_min[curr_note.ntl] = kp_min[i];
	}*/

	// The note-sequence / rscd weights
	// Each rscd is weighted according to the total amount of time it occupies 
	// in the sequence.  Chords are flattened (TODO:  Not sure how Temperley
	// deals with chords).  
	// rscds_weighted[0] => total duration for 'C', rscds_weighted[1] => C#, ...
	// Relies on the fact that for spn12tet, C => rscd == 0, etc.  
	auto notes_flatchords_norests = nts.notes_flat();

	dbk::contigumap<ntl_t,double> nsw {cchrom_ntls,std::vector<double>(12,0.0)};  // "note-string-weighted"
	//dbk::contigumap<rscdoctn_t,double> rw {};
	std::vector<double> rscds_weighted(12,0.0);
	d_t dw {d::w};
	for (size_t i=0; i<notes_flatchords_norests.size(); ++i) {
		ntstr_t curr_ntstr {notes_flatchords_norests[i].note.ntl,notes_flatchords_norests[i].note.oct};
		int curr_rscd = rscdoctn_t{sc.to_scd(curr_ntstr),12}.to_int();
		rscds_weighted[curr_rscd] += (notes_flatchords_norests[i].d)/dw;

		auto curr_ntl = notes_flatchords_norests[i].note.ntl;
		//rscdoctn_t cr {sc.to_scd(notes_flatchords_norests[i].note),12};
		//rw[cr] += (notes_flatchords_norests[i].d)/dw;
		nsw[curr_ntl] += (notes_flatchords_norests[i].d)/dw;
	}

	/*
	auto corr_maj = corr(rscds_weighted,kp_maj);
	auto corr_min = corr(rscds_weighted,kp_min);
	auto rscd_max_corr_maj = std::distance(
		corr_maj.begin(),
		std::max_element(corr_maj.begin(), corr_maj.end()));
	auto rscd_max_corr_min = std::distance(
		corr_min.begin(),
		std::max_element(corr_min.begin(), corr_min.end()));
		*/
	auto corr_maj_frommap = nsw;
	auto corr_min_frommap = nsw;
	for (const auto& curr_kp : kp_maj) {
		auto curr_corr = corr(nsw.values(),curr_kp.v);
		corr_maj_frommap[curr_kp.k] = curr_corr;
	}
	for (const auto& curr_kp : kp_min) {
		auto curr_corr = corr(nsw.values(),curr_kp.v);
		corr_min_frommap[curr_kp.k] = curr_corr;
	}
	auto max_corr_maj_frommap = *std::max_element(
		corr_maj_frommap.begin(),corr_maj_frommap.end(),
		[](const dbk::contigumap<ntl_t,double>::kvpair_t& lhs, 
			const dbk::contigumap<ntl_t,double>::kvpair_t& rhs){
				return lhs.v < rhs.v;
			}
		);
	auto max_corr_min_frommap = *std::max_element(
		corr_min_frommap.begin(),corr_min_frommap.end(),
		[](const dbk::contigumap<ntl_t,double>::kvpair_t& lhs, 
			const dbk::contigumap<ntl_t,double>::kvpair_t& rhs){
				return lhs.v < rhs.v;
			}
		);

	//
	// An alternative way, but assumes nsw contains all the ntls; if not, corr_[]_frommap
	// will have an unexpected size...
	// or maybe it won't work at all...
	//for (const auto& curr_ntl : nsw) {
	//	auto corr_maj_frommap = corr(nsw.values(),mkp_maj[curr_ntl.k]);
	//	auto corr_min_frommap = corr(nsw.values(),mkp_min[curr_ntl.k]);
	//}

	//std::cout << printm(kp_maj,"%10.3f") << std::endl << std::endl;
	//std::cout << printm(kp_min,"%10.3f") << std::endl << std::endl;

	//ks_key_result res {};
	/*if (corr_maj[rscd_max_corr_maj] >= corr_min[rscd_max_corr_min]) {
		// NOTE:  >= => report major if there is a tie
		res.key = sc.to_ntstr(scd_t{static_cast<int>(rscd_max_corr_maj)}).ntl();
		res.ismajor = true;
		res.score = corr_maj[rscd_max_corr_maj];
	} else {
		res.key = sc.to_ntstr(scd_t{static_cast<int>(rscd_max_corr_min)}).ntl();
		res.ismajor = false;
		res.score = corr_maj[rscd_max_corr_min];
	}*/

	ks_key_result res {};
	if (max_corr_maj_frommap.v >= max_corr_min_frommap.v) {
		// NOTE:  >= => report major if there is a tie
		res.key = max_corr_maj_frommap.k;  //sc.to_ntstr(scd_t{static_cast<int>(rscd_max_corr_maj)}).ntl();
		res.ismajor = true;
		res.score = max_corr_maj_frommap.v;  //corr_maj[rscd_max_corr_maj];
	} else {
		res.key = max_corr_min_frommap.k;  //sc.to_ntstr(scd_t{static_cast<int>(rscd_max_corr_min)}).ntl();
		res.ismajor = false;
		res.score = max_corr_min_frommap.v;  //corr_maj[rscd_max_corr_min];
	}

	/*for (int i=0; i<12; ++i) {
		res.all_scores[0][i] = corr_maj[i];
		res.all_scores[1][i] = corr_min[i];
	}*/
	for (const auto& curr_ntl : kp_maj) {
		res.all_scores[curr_ntl.k][0] = corr_maj_frommap[curr_ntl.k];
		res.all_scores[curr_ntl.k][1] = corr_min_frommap[curr_ntl.k];
	}

	return res;
}


