#pragma once
#include "..\types\scd_t.h"
#include "..\types\line_t.h"
#include "..\types\ntl_t.h"
#include "..\types\frq_t.h"
#include "..\types\ts_t.h"
#include "..\types\beat_bar_t.h"
#include "dbklib\contigumap.h"
#include <vector>


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
namespace melody_hiller_internal {

using hiller_melody = std::vector<std::vector<note_t>>;

// Status object to keep track of the growing melody
struct hiller21_status {
	int nnts {24};  // number of notes (== number of chords if nvoices > 1)
	int nvoices {3};
	int ch_idx {0};  // The current working chord
	int v_idx {0};  // The current working note
	note_t new_nt {};
	int rejects_curr_ch {0};  // Zeroed upon changing ch_idx
	int rejects_tot {0};  // Never zeroed
	int full_resets_tot {0};
	std::vector<int> rule {};
		// Expands dynamically such that rule.size()==(r-1) for the largest r passed
		// to set_result(r).  Set to 0 by clear_rules().  
		// set_result(r) sets rule[r-1];  Melody passes rule => 1, fails rule => -1,
		// rule not evaluated => 0

	bool first_ch() const;  // True if ch_idx == 0
	bool last_ch() const;  // True if ch_idx == nnts-1
	bool first_v() const;  // True if v_idx == 0
	bool last_v() const;  // True if v_idx == nvoices-1
	bool any_failed() const;  // True iff 1 or more of rule ==-1
	void set_result(size_t, bool);  // True => melody _passes_ rule
	void clear_rules();
	void set_for_new_attempt_curr_nt();  // ++rejects_curr_ch;  ++rejects_tot
	void set_for_new_attempt_prev_ch();  // rejects_curr_ch=0; ++rejects_tot; ch_idx-=1; v_idx=0
	void set_for_new_attempt_mel();  // rejects_curr_ch=0; ++rejects_tot; v_idx=0; ch_idx=0
	void set_for_next();  // rejects_curr_ch=0; ++v_idx; ch_idx+=1 or =0 depending
	std::string print(const std::vector<std::vector<note_t>>&) const;
};


// Utility functions, required by the rule functions

note_t max_frq (const std::vector<note_t>&);
note_t min_frq (const std::vector<note_t>&);
int abs_staffdiff_cmn(const note_t&,const note_t&);
int abs_semidiff(const note_t&,const note_t&);
bool ntlo_eq(const note_t&, const note_t&);
bool ntl_lt_byfrq(const note_t&, const note_t&);

std::vector<note_t> get_chord(const hiller21_status&, const hiller_melody&, const int);
std::vector<note_t> get_voice(const hiller21_status&, const hiller_melody&, const int);
bool ch_contains_tritone (const std::vector<note_t>&);

// Rules
// Rule 1
bool line_spans_gt_oct(const hiller21_status&, const hiller_melody&, const note_t&);
// Rule 2
bool cf_beginend_tonic(const hiller21_status&, const hiller_melody&, const note_t&);
// Rule 3
bool noncf_beginend_tonictriad(const hiller21_status&, const hiller_melody&, const note_t&);
// Rule 4
bool no_mel_sevenths(const hiller21_status&, const hiller_melody&, const note_t&);
// Rule 5
bool skip_step_rule(const hiller21_status&, const hiller_melody&, const note_t&);
// Rule 6
bool rpts_gt_one(const hiller21_status&, const hiller_melody&, const note_t&);
// Rule 7
bool rpt_high_nt(const hiller21_status&, const hiller_melody&, const note_t&);
// Rule 8
bool harmonic_consonant(const hiller21_status&, const hiller_melody&, const note_t&);
// Rule 9
bool harmonic_p4(const hiller21_status&, const hiller_melody&, const note_t&);
// Rule 10
bool d_below_tritone(const hiller21_status&, const hiller_melody&, const note_t&);
// Rule 11
bool beginend_tonictriad_rootpos(const hiller21_status&, const hiller_melody&, const note_t&);
// Rule 12a
bool ch_nxtlast_contains_b(const hiller21_status&, const hiller_melody&, const note_t&);
// Rule 12b
bool lastch_contains_rsln_from_b(const hiller21_status&, const hiller_melody&, const note_t&);






};  // namespace melody_hiller_internal

struct melody_hiller_params {
	int nnts {24};  // nnts per voice
	int nvoice {3};
	std::string min {"C(3)"};  // Must be valid for Cmaj-SPN
	std::string max {"C(5)"};
	int max_rejects_tot {10000};  // Before aborting the entire operation
	int rejects_regen_ch {20};  // Before dropping+regenerating the prev. ch
	int debug_lvl {3};
		// 0 => No messages
		// 1 => Only the melody
		// 2 => Melody on successfull iterations only
		// 3 => Melody on also on failed iterations
		// 4 => ??
};
std::vector<std::vector<note_t>> melody_hiller_ex21(const melody_hiller_params&);  // "Experiment 2, part 1"


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






