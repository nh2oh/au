#include "randmel_gens.h"
#include "..\util\au_random.h";
#include "..\scale\diatonic_spn.h";
#include "..\types\ntl_t.h";
#include "..\types\frq_t.h";
#include <string>
#include <vector>
#include <iostream>
#include <exception>
#include <algorithm>
#include <random>
#include <cmath>

/*
std::vector<note_t> melody_hiller_ex11(const melody_hiller_params& p) {
	// 1)  No tritones
	// 2)  No sevenths
	// 3)  Start, end on middle C
	// 4)  max(melody)-min(melody) <= 1 octave

	bool passed {false};
	while (!passed) {
		//...
	}

}*/



//
// It should also be possible to implement this in the same way as melody_a(), ie, by first
// generating a completely random 2-d array of notes, scoring according to the counterpoint
// rules, mutating, re-scoring, etc...
//
// Better method:  Use the rules to set the ntpool, then any selection will work.  
//
std::vector<note_t> melody_hiller_ex21(const melody_hiller_params& p) {
	diatonic_spn sc {ntl_t {"C"}, diatonic_spn::mode::major};
	int nnts {12};
	int nvoices {2};
	int scd_min = sc.to_scd(ntl_t{"C"},octn_t{3});
	int scd_max = sc.to_scd(ntl_t{"C"},octn_t{5});

	struct m_status {
		int nnts {12};  // TODO:   Not the best name... nchs?
		int nvoices {2};
		// Position 'counters' are incremented in such a way that at all locations in 
		// the loop, m[v_idx][ch_idx] is valid.  
		int ch_idx {0};  // TODO:  vi,ci ??  v,c ??
		int v_idx {0};

		int nt_rejects_curr_voice {0};  
		int nt_rejects_curr_chord {0};
		int nt_rejects_tot {0};  
		int ch_rejects_curr_ch {0};  // # of discards of the _entire_ completed chord
		int ch_rejects_tot {0};
	};
	m_status midx {};  // Current position in the developing melody

	std::vector<note_t> ntpool {};
	for (int s = scd_min; s<=scd_max; ++s) {
		ntpool.push_back(sc[s]);
	}
	std::vector<note_t> ntpool_tonic_nt {};
	std::vector<note_t> ntpool_tonic_ch {};
	for (const auto& e : ntpool) {
		if (e.ntl == ntl_t {"C"}) {
			ntpool_tonic_nt.push_back(e);
		}
		if (e.ntl == ntl_t {"C"} || e.ntl == ntl_t {"E"} || e.ntl == ntl_t {"G"}) {
			ntpool_tonic_ch.push_back(e);
		}
	}

	auto re = new_randeng(true);
	std::uniform_int_distribution rd {scd_min, scd_max};

	// m[voice][note]
	std::vector<std::vector<note_t>> m {};
	m.insert(m.begin(),nvoices,std::vector<note_t>(nnts,sc[0]));
	
	auto min_frq = [](const std::vector<note_t>& nts) -> note_t {
		return *std::min_element(nts.begin(),nts.end(),
			[](const note_t& lhs, const note_t& rhs){return rhs.frq<rhs.frq;});
	};
	auto max_frq = [](const std::vector<note_t>& nts) -> note_t {
		return *std::max_element(nts.begin(),nts.end(),
			[](const note_t& lhs, const note_t& rhs){return rhs.frq<rhs.frq;});
	};
	// Usefull for using std::find on a std::vector<note_t> to find a particular ntl
	auto ntl_eq = [](const note_t& lhs, const note_t& rhs) -> bool {
		return lhs.ntl == rhs.ntl;
	};
	auto ntl_ismember = [](const std::vector<note_t>& nts, const ntl_t& nt) -> bool {
		for (int i=0; i<nts.size(); ++i) {
			if (nts[i].ntl==nt) {
				return true;
			}
		}
		return false;
	};

	// m[voice][note]
	auto get_chord = [&midx, &m](int chord_idx) -> std::vector<note_t> {
		std::vector<note_t> result {}; result.reserve(midx.nvoices);
		for (int i=0; i<midx.nvoices; ++i) {  // m.size() == nvoices
			result.push_back(m[i][chord_idx]);
		}
		return result;
	};
	// m[voice][note]
	auto get_voice = [&m](int voice_idx) -> std::vector<note_t> {
		return m[voice_idx];
	};

	// True if any note in the second-to-last and last chords of m contain a B and F, or an F and B,
	// respectively.  That is, if there is a tritone betweeen any two voices in the last two chords.
	// False otherwise
	auto tritone = [&sc, &m, get_chord, ntl_ismember](int last_ch_idx) -> bool {
		if (last_ch_idx <= 1) { return false; }
		auto ch_last = get_chord(last_ch_idx);
		auto ch_nxtlast = get_chord(last_ch_idx-1);
		return (ntl_ismember(ch_last,ntl_t{"F"}) &&	ntl_ismember(ch_nxtlast,ntl_t{"B"}) ||
			ntl_ismember(ch_last,ntl_t{"B"}) &&	ntl_ismember(ch_nxtlast,ntl_t{"F"}));
	};

	auto num_interval = [&sc](const note_t& from, const note_t& to) -> int {
		return sc.to_scd(to)-sc.to_scd(from);
	};

	// Rule 1
	// The max interval spanned by the lowest and highest notes on a given line must 
	// be <= 1 octave.  
	auto span_geq_oct = [&m,&midx,min_frq,max_frq](const note_t& new_nt) -> bool { 
		if (midx.v_idx <= 1) { return false; }
		double ratio_lowest = (new_nt.frq)/(min_frq(m[midx.v_idx]).frq);
		double ratio_highest = (max_frq(m[midx.v_idx]).frq)/(new_nt.frq);
		return (ratio_lowest > 2.0 || ratio_highest > 2.0);
	};

	// Rule 2 - Implemented implictly
	// Rule 3 - Implemented implictly
	// Rule 4 - Direct call to seventh()

	// Rule 5
	// For the current voice, if the prev. two notes are >= an m3 apart (a "skip"), new_nt must 
	// be either a repeat of the prev nt or differ from the prev nt by an m2 (a "step").  If the 
	// prev two notes are a step, new_nt must be either a step or a skip.  
	auto skip_step_rule = [&m,&midx,num_interval](const note_t& new_nt) -> bool { 
		if (midx.ch_idx <= 1) { return false; }
		auto ci = midx.ch_idx;
		auto vi = midx.v_idx;
		int prev_int = num_interval(m[vi][ci-1],m[vi][ci]);
		int new_int = num_interval(m[vi][ci],new_nt);
		if (std::abs(prev_int) >= 3) {  // Prev 2 notes are a "skip"
			return !(new_nt.ntl == m[vi][ci].ntl || std::abs(new_int)==2);
		} else if (std::abs(prev_int)==2) {  // Prev 2 notes are a "step"
			return !(std::abs(new_int)==2 || std::abs(new_int)==3);
		}
		return false;
	};

	// Rule 6
	// No more than one successive repeat of a note is allowed in a given voice
	auto rpts_gt_one = [&m,&midx](const note_t& new_nt) -> bool { 
		if (midx.ch_idx <= 1) {	return false; }
		return (m[midx.v_idx][midx.ch_idx-1].ntl == new_nt.ntl 
			&& m[midx.v_idx][midx.ch_idx].ntl == new_nt.ntl);
	};

	// Rule 7
	// Forbidden to repeat the highest note of a line unless said note is the tonic AND
	// the line moves in a tritone resolution or a cadence to the high tonic.  
	// At present only checks for repeats of the highest note on a given line and that
	// said repeat is of C.  
	// Rule is only defined on a completed line, since only then can i define the "highest
	// note."  Since the melody is generated one chord at a time, the lines are all completed
	// together with the full melody.  
	auto rpt_high_nt = [&m,&midx,max_frq]() -> bool { 
		if (midx.ch_idx < midx.nnts || midx.v_idx < midx.nvoices) {
			// This function can only be called on a completed melody
			std::abort();
		}
		for (int i=0; i<m.size(); ++i) {  // i ~ current voice
			note_t max_nt = max_frq(m[i]);  // Highest nt of voice i
			for (int j=0; j<(m[i].size()-1); ++j) {  // j ~ current nt of voice i
				if (m[i][j] == max_nt && m[i][j+1] == max_nt) {
					return true;
				}
			}  // to next nt j of voice i
		}  // to next voice i
		return false;
	};

	// Rule 8
	// For each chord, the only harmonic intervals permitted are U, O, P5, m3, M3, 6'ths.  
	// Forbidden are m2, M2, m7, M7, tritone
	// TODO:  How to include tritone?
	auto harmonic_consonant = [&m,&midx,num_interval](const note_t& new_nt) -> bool {
		if (midx.ch_idx < 1 || midx.v_idx == 0) { return false; }
		for (const auto& e : m[midx.v_idx]) {
			if (std::abs(num_interval(e,new_nt)) == 7  // m7, M7
				|| std::abs(num_interval(e,new_nt)) == 2) { // M2, m2
				return true;
			}
		}
		return false;
	};

	// Rule 9
	// For each chord, a P4 is allowed iff it does not occur between the lowest note of the
	// chord and an upper voice.  
	// This rule returns false unless new_nt completes the chord, in which case it is 
	// is evaluated and may return true.  
	auto harmonic_p4 = [&m,&midx,num_interval](const note_t& new_nt) -> bool {
		if (midx.v_idx != midx.nvoices) { return false; }
		// new_nt is the lowest note of the chord
		for (const auto& e : m[midx.v_idx]) {
			if (std::abs(num_interval(e,new_nt)) == 4) {
				return true;
			}
		}
		return false;
	};

	// Rule 12 - part a, part b
	// The second-last chord must contain the note B in exactly one voice.  In the last chord,
	// this voice must be the note C.  
	// Always returns false unless midx indicates that the second-to-last chord has been 
	// completed, at which point returns true if the rule is violated.  
	auto ch_nxtlast_contains_b = [&m,&midx,get_chord,ntl_ismember]() -> bool {
		if (midx.v_idx != midx.nvoices  // chord is complete
			|| midx.ch_idx != (midx.nnts-1)) { return false; }
		std::vector<note_t> ch = get_chord(midx.ch_idx);
		int num_b {0};
		for (const auto& e : ch) {
			if (e.ntl == ntl_t{"B"}) { ++num_b; }
		}
		return num_b != 1;
	};
	auto lastch_contains_rsln_from_b = [&m,&midx,get_chord]() -> bool {
		if (midx.v_idx != midx.nvoices  // chord is complete
			|| midx.ch_idx != midx.nnts) { return false; }
		std::vector<note_t> ch_nxtlast = get_chord(midx.ch_idx-1);
		std::vector<note_t> ch_last = get_chord(midx.ch_idx);
		for (int vi=0; vi<midx.v_idx; ++vi) {
			if (ch_nxtlast[vi].ntl == ntl_t{"B"}) { 
				return ch_last[vi].ntl != ntl_t {"C"};
				// == C => rule passes => return false
			}
		}
		// Means ch_nxtlast did not contain a B - rule 12a should prevent this from
		// happening.  
		return true;  
	};

	std::vector<note_t> curr_ntpool {};
	// Position 'counters' are incremented in such a way that at all locations in 
	// the loop, m[v_idx][ch_idx] is valid.  
	int ch_idx {0}; int v_idx {0};
	while (midx.ch_idx < midx.nnts) { //for (int i=0; i<nnts; ++i) {  // m[voice][note]
		while (midx.v_idx < midx.nvoices) { //for (int j=0; j<nvoices; ++j) {  // m[voice][note]
			//
			// Set curr_ntpool
			//
			curr_ntpool = ntpool;
			if (ch_idx==0) {  // first chord of the melody
				if (v_idx==0) {  // cf
					// Rule 2:  cf begins+ends on the tonic
					curr_ntpool = ntpool_tonic_nt;
				} else if (v_idx < nvoices) {  
					// Rule 3, Rule 11:  
					// Non-cf voices begin+end on tonic triad root position notes
					curr_ntpool = ntpool_tonic_ch;
				} else if (v_idx == nvoices) { // The lowest voice
					// Rule 3, Rule 11:  
					// The first chord must be based on the tonic chord root position
					curr_ntpool = ntpool_tonic_nt;
				}
			} else if (ch_idx == nnts-1) {  // The second-last ch of the melody
				curr_ntpool = ntpool;
			} else if (ch_idx == nnts) {  // The final chord of the melody
				if (v_idx == 0) {  // cf
					// Rule 2:  The cf begins+ends on the tonic
					curr_ntpool = ntpool_tonic_nt;
				} else if (v_idx < nvoices) {
					// Rule 3, Rule 11:  
					// The final chord must be based on the tonic chord root position
					curr_ntpool = ntpool_tonic_ch;
				} else if (v_idx == nvoices) { // The lowest voice
					// Rule 3, Rule 11:  
					// The final chord must be based on the tonic chord root position
					curr_ntpool = ntpool_tonic_nt;
				}
			} else { // not the first or last chord of the melody
				//...
			}

			//
			// Draw potential note m[v_idx][ch_idx]
			//
			note_t new_nt = curr_ntpool[rd(re)%curr_ntpool.size()];

			//
			// Compare new_nt against the rule set
			//
			// Rules 2,3,11 implicit through ntpool.  Rule 7 defined only on the completed melody
			//
			if (std::abs(num_interval(m[v_idx][ch_idx],new_nt))==7  // Rule 4
				|| span_geq_oct(new_nt)  // Rule 1
				|| rpts_gt_one(new_nt)   // Rule 6
				|| skip_step_rule(new_nt)   // Rule 5
				|| harmonic_consonant(new_nt)  // Rule 8
				|| harmonic_p4(new_nt))  // Rule 9
			{  
				// reject new_nt; midx.v_idx not incremented
				midx.nt_rejects_curr_voice += 1;
				midx.nt_rejects_curr_chord += 1;
				midx.nt_rejects_tot += 1;
			} else {
				// accept the new nt by moving to the next voice
				midx.v_idx += 1;
				midx.nt_rejects_curr_voice = 0;
			}
		}  // to next voice of present chord
		midx.nt_rejects_curr_chord = 0;

		if (ch_nxtlast_contains_b() || lastch_contains_rsln_from_b()) { // Rule 12
			// reject the present chord
			midx.ch_rejects_curr_ch += 1;
			midx.ch_rejects_tot += 1;
		} else {
			// accept the present chord
			midx.ch_idx += 1;
			midx.ch_rejects_curr_ch = 0;
		}
	}  // to next chord of first voice


	if (rpt_high_nt()) {  // Rule 7
		// discard melody
	}

}
