#pragma once
#include "..\types\line_t.h"
#include "..\types\ntl_t.h"
#include "..\types\frq_t.h"
#include "..\types\ts_t.h"
#include "..\types\beat_bar_t.h"
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
	scd_t min {54};  // All scds in the generated sequence are between 
	scd_t max {88};  // these completely made these up values.  

	int npass {100};
	double sc_adjnts {0};
	double sc_rptnts {-0.5};  // For each adjacent pair of == scds
	double sc_stepsize {-0.21};  // Per unit deviation from optimstep
	//double sc_keyscore {-1};
	double optimstep {1.5};
};
std::vector<ntstr_t> melody_a(ma_params);


//
// kk_key()
//
struct kk_key_params {
	int profile {0}; // 0 => kk, 1 => temperley
};
struct kk_key_result {
	scd_t key {0};
	bool ismajor {false};
	double score {0};
	bool istie {false};
	std::array<std::array<double,12>,2> 
		all_scores {{{0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}}};
};

kk_key_result kk_key(line_t<ntstr_t>,kk_key_params);






