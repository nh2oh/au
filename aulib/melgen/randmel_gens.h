#pragma once
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
line_t<ntstr_t> melody_temperley(ntl_t,bool,ts_t,bar_t);


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
	//bool istie {false};
	dbk::contigumap<ntl_t,ks_key_result::major_minor_pair> all_scores {};
};

ks_key_result ks_key(line_t<note_t>,ks_key_params);






