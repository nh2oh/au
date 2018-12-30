#pragma once
#include "..\types\ntl_t.h"
#include "..\scale\diatonic_spn.h"   // declaration of the hiller status object
#include "..\types\rp_t.h"
#include <vector>
#include <string>

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
	int nnts {24};  // nnts per voice
	int nvoice {3};
	std::string min {"C(3)"};  // Must be valid for Cmaj-SPN
	std::string max {"C(5)"};
	int max_rejects_tot {10000};  // Before aborting the entire operation
	int rejects_regen_ch {20};  // Before dropping+regenerating the prev. ch
	int debug_lvl {3};
		// 0 => No messages, not even the melody
		// 1 => Only the melody, no messages (failure => no output)
		// 2 => Melody + summary message
		// 3 => Progress each successfull iteration; melody + summary message
		// 4 => Progress each iteration;  success/failure message
		// 5 => ??

	bool use_rp {false};
};
std::vector<std::vector<note_t>> melody_hiller_ex21(const melody_hiller_params&);  // "Experiment 2, part 1"
std::vector<std::vector<int>> rhythm_hiller_ex31(const melody_hiller_params&);
std::vector<std::vector<int>> rhythm_hiller_ex32(const melody_hiller_params&);
std::vector<rp_t> hillerrp2dbkrp(const std::vector<std::vector<int>>&);

namespace melody_hiller_internal {

using hiller_melody = std::vector<std::vector<note_t>>;

// Status object to keep track of the growing melody
struct hiller21_status {
	diatonic_spn *cmaj;

	int nnts {24};  // number of notes (== number of chords if nvoices > 1)
	int nvoices {3};
	int ch_idx {0};  // The current working chord
	int v_idx {0};  // The current working note
	note_t new_nt {};

	int rejects_curr_ch {0};  // Zeroed upon changing ch_idx
	int rejects_tot {0};  // Never zeroed
	int full_resets_tot {0};
	std::vector<int> rule {};  // Pass/fail/not-evaluated result for each rule
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
};  // struct hiller21_status


// Utility functions, required by the rule functions

note_t max_frq (const std::vector<note_t>&);
note_t min_frq (const std::vector<note_t>&);
bool ntlo_eq(const note_t&, const note_t&);
bool ntl_lt_byfrq(const note_t&, const note_t&);

std::vector<note_t> get_chord(const hiller21_status&, const hiller_melody&, const int);
std::vector<note_t> get_voice(const hiller21_status&, const hiller_melody&, const int);
bool ch_contains_tritone (const std::vector<note_t>&);
int last_nonzero_idx(const std::vector<std::vector<int>>&, int, int);

// Rules
// Rule 1
bool line_spans_gt_oct(const hiller21_status&, const hiller_melody&);
void line_spans_gt_oct_allowed(const hiller21_status&, const hiller_melody&, std::vector<note_t>&);
// Rule 2
bool cf_beginend_tonic(const hiller21_status&, const hiller_melody&);
// Rule 3
bool noncf_beginend_tonictriad(const hiller21_status&, const hiller_melody&);
// Rule 4
bool no_mel_sevenths(const hiller21_status&, const hiller_melody&);
// Rule 5
bool skip_step_rule(const hiller21_status&, const hiller_melody&);
void skip_step_allowed(const hiller21_status&, const hiller_melody&, std::vector<note_t>&);
// Rule 6
bool rpts_gt_one(const hiller21_status&, const hiller_melody&);
// Rule 7
bool rpt_high_nt(const hiller21_status&, const hiller_melody&);
// Rule 8
bool harmonic_consonant(const hiller21_status&, const hiller_melody&);
// Rule 9
bool harmonic_p4(const hiller21_status&, const hiller_melody&);
// Rule 10
bool d_below_tritone(const hiller21_status&, const hiller_melody&);
// Rule 11
bool beginend_tonictriad_rootpos(const hiller21_status&, const hiller_melody&);
// Rule 12
bool ending_cadence(const hiller21_status&, const hiller_melody&);
// Rule 12a (only used internally by Rule 12)
bool ch_nxtlast_contains_b(const hiller21_status&, const hiller_melody&);
// Rule 12b (only used internally by Rule 12)
bool lastch_contains_rsln_from_b(const hiller21_status&, const hiller_melody&);


};  // namespace melody_hiller_internal


