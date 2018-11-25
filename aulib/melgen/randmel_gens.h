#pragma once
#include "..\types\scd_t.h"
#include "..\types\line_t.h"
#include "..\types\ntl_t.h"
#include "..\types\frq_t.h"
#include "..\types\ts_t.h"
#include "..\types\beat_bar_t.h"
#include "dbklib\contigumap.h"
#include <vector>
#include <array>


//
// melody_temperley()
//
struct melody_temperley_params {
	ntl_t key_ntl {"C"};
	int nnts {100};
	double p_major {0.88};
	int sz_scdpool {120};  // scd's on:  [0 sz_scdpool)
	int CP_mean {48};
	int CP_stdev {13};
	int RP_stdev {5};
	int PP_stdev {7};   // double PP_stdev = 7.2;
};  //TODO:  These stdev and mean params should be doubles...
std::vector<note_t> melody_temperley(const melody_temperley_params&);

// Key ntl, nnts
std::vector<note_t> melody_temperley(ntl_t, int);


//
// melody_a()
//
struct ma_params {
	int nnts {32};
	scd_t min {54};  // All scds in the generated sequence fall on [min,max]
	scd_t max {88};  
	int npass {100};
	
	// Scoring parameters:  (-) scores => a worse melody
	double sc_adjnts {0};
	double sc_rptnts {-0.5};  // For each adjacent pair of == scds
	double sc_stepsize {-0.21};  // Per unit deviation from optimstep
	double optimstep {1.5};
};
std::vector<scd_t> melody_a(ma_params);


//
// melody_hiller()
//
// Based on the description by Hiller & Isaacson on p. 74-... of "Experimental 
// Music Composition with an Electronic Computer)" 1959
//
// Their alg works on the white notes of the piano, hence the internal scale is
// diatonic_spn, and i return note_t's, not scd_t's.  
//
//
struct melody_hiller_params {
	int nnts {12};
	std::string min {"C(3)"};
	std::string max {"C(5)"};
};
std::vector<note_t> melody_hiller(const melody_hiller_params&);



//
// ks_key()
//
struct ks_key_params {
	int profile {0}; // 0 => kk, 1 => temperley
};

struct ks_key_result {
	struct major_minor_pair {
		double maj {0.0};
		double min {0.0};
	};

	ntl_t key {};
	bool ismajor {false};
	double score {0};
	dbk::contigumap<ntl_t,ks_key_result::major_minor_pair> all_scores {};
};

ks_key_result ks_key(line_t<note_t>,ks_key_params);






