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
#include <array>

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
std::vector<std::vector<note_t>> melody_hiller_ex21() {
	diatonic_spn sc {ntl_t {"C"}, diatonic_spn::mode::major};
	int scd_min = sc.to_scd(ntl_t{"C"},octn_t{3});
	int scd_max = sc.to_scd(ntl_t{"C"},octn_t{5});

	struct m_status {
		int nnts {12};  // TODO:   Not the best name... nchs?
		int nvoices {2};

		int ch_idx {0};  // TODO:  vi,ci ??  v,c ??
		int v_idx {0};
		note_t new_nt {};

		int nt_rejects_curr_v {0};  
		int nt_rejects_curr_ch {0};  // either voice of present ch_idx 
		int nt_rejects_tot {0};  
		int ch_rejects_curr_ch {0};  // # of discards of the _entire_ completed ch
		int ch_rejects_tot {0};

		std::array<int,20> rule {0};  // -1=>fail, 0=>ne, 1=>pass

		void set_rule_result(size_t r, bool pass) { rule[r-1] = pass; }
		void set_for_new_nt_attempt() { rule.fill(0); }
		void set_for_next_nt() {
			rule.fill(0);
			nt_rejects_curr_v = 0;
		}
		void set_for_next_ch_attempt() {
			rule.fill(0);
			nt_rejects_curr_v = 0;
			nt_rejects_curr_ch = 0;
		}
		void set_for_next_ch() {
			rule.fill(0);
			nt_rejects_curr_v = 0;
			nt_rejects_curr_ch = 0;
			ch_rejects_curr_ch = 0;
		}
		
		std::string print(const std::vector<std::vector<note_t>>& m) { 
			std::string s {};

			for (int v=0; v<nvoices; ++v) {  // v<nvoices => always print 2 rows
				s += v + ":  ";
				for (int ch=0; ch<ch_idx; ++ch) {  // ch<ch_idx => don't print past current working idx
					s += m[v][ch].print() + "  ";
				}
				if (v == v_idx) { s+= "<-" + new_nt.print(); }
				s += "\n";
			}

			std::string failstr {};	std::string passstr {};	std::string nestr {};
			bool found_failed {false}; bool found_passed {false}; bool found_ne {false};
			for (int i=0; i<rule.size(); ++i) {
				if (rule[i] == -1) {
					if (!found_failed) { failstr += "Failed: "; }
					if (found_failed) { failstr += ", "; }  // add sep iff have found prior
					failstr += std::to_string(i-1);
					found_failed = true;
				} else if (rule[i] == 0) {
					if (!found_ne) { nestr += "Not Evaluated: "; }
					if (found_ne) { nestr += ", "; }  // add sep iff have found prior
					nestr += std::to_string(i-1);
					found_ne = true;
				} else if (rule[i] == 1) {
					if (!found_passed) { passstr += "Passed: "; }
					if (found_passed) { passstr += ", "; }  // add sep iff have found prior
					passstr += std::to_string(i-1);
					found_passed = true;
				}
			}
			s += passstr + "\n" + failstr + "\n" + nestr + "\n";
			if (found_failed) {
				s += "Overall Failed\n";
			} else {
				s += "Overall Passed\n";
			}

			return s;
		}
	};
	m_status midx {};  // Current position in the developing m

	std::vector<note_t> ntpool {};
	for (int s = scd_min; s<=scd_max; ++s) {
		ntpool.push_back(sc[s]);
	}

	auto re = new_randeng(true);
	std::uniform_int_distribution rd {size_t {0}, ntpool.size()};

	// m[voice][note]
	std::vector<std::vector<note_t>> m {};
	m.insert(m.begin(),midx.nvoices,std::vector<note_t>(midx.nnts,sc[0]));
	
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
	// Used in calling std::sort() on a std::vector<note_t> to sort by frq
	auto ntl_lt_frq = [](const note_t& lhs, const note_t& rhs) -> bool {
		return lhs.frq < rhs.frq;
	};
	auto ntl_ismember = [](const std::vector<note_t>& nts, const ntl_t& nt) -> bool {
		for (int i=0; i<nts.size(); ++i) {
			if (nts[i].ntl==nt) {
				return true;
			}
		}
		return false;
	};

	// Gets from v_idx == 0 to midx.v_idx if requesting chord_idx == midx.ch_idx, or from
	// v_idx == 0 to midx.nvoices if chord_idx < midx.ch_idx.  
	auto get_chord = [&midx, &m](int chord_idx) -> std::vector<note_t> {
		int max_v_idx {0};
		if (chord_idx > midx.ch_idx) {
			std::abort();
		} else if (chord_idx < midx.ch_idx) {
			// Requesting a ch prior to the working ch
			max_v_idx = midx.nvoices;
		} else {  // (chord_idx == midx.ch_idx)
			// Requesting the working ch 
			max_v_idx = midx.v_idx;
		}

		std::vector<note_t> ch {};
		for (int i=0; i<max_v_idx; ++i) {  // m.size() == nvoices
			ch.push_back(m[i][chord_idx]);  // m[voice][note]
		}
		return ch;
	};

	// 1 => m2 || M2
	// 2 => m3 || M3
	// 3 => 
	auto num_interval = [&sc](const note_t& from, const note_t& to) -> int {
		return sc.to_scd(to)-sc.to_scd(from);
	};

	// Rule 1
	// The max interval spanned by the lowest and highest notes on a given line must 
	// be <= 1 oct.  
	auto span_geq_oct = [&m,&midx,min_frq,max_frq](const note_t& new_nt) -> bool { 
		if (midx.ch_idx == 0) { return false; }
		double ratio_lowest = (new_nt.frq)/(min_frq(m[midx.v_idx]).frq);
		double ratio_highest = (max_frq(m[midx.v_idx]).frq)/(new_nt.frq);
		return (ratio_lowest > 2.0 || ratio_highest > 2.0);
	};

	// Rule 2
	// cf begins+ends on the tonic
	auto cf_beginend_tonic = [&m,&midx](const note_t& new_nt) -> bool { 
		if (midx.ch_idx > 0 || midx.ch_idx < (midx.nnts-1) 
			|| midx.v_idx > 0) { 
			return false;
		}
		return new_nt.ntl != ntl_t {"C"};
	};

	// Rule 3, 11
	// Non-cf voices begin+end on tonic triad root position notes
	auto noncf_beginend_tonictriad = [&m,&midx](const note_t& new_nt) -> bool { 
		if (midx.ch_idx > 0 || midx.ch_idx < (midx.nnts-1) 
			|| midx.v_idx == 0) { 
			return false;
		}

		if (midx.v_idx == (midx.nvoices-1)) {  // The lowest voice
			return new_nt.ntl != ntl_t {"C"};
		}
		return !(new_nt.ntl == ntl_t {"C"} || new_nt.ntl == ntl_t {"E"} 
			|| new_nt.ntl == ntl_t {"G"});
	};



	// Rule 4
	// For any voice, no intervals == m7 || M7
	auto no_mel_sevenths = [&m,&midx,num_interval](const note_t& new_nt) -> bool { 
		if (midx.ch_idx == 0) { return false; }
		return std::abs(num_interval(m[midx.v_idx][(midx.ch_idx-1)],new_nt))==6;
	};

	// Rule 5
	// For the current voice, if the prev. two notes are >= an m3 apart (a "skip"), new_nt must 
	// be either a repeat of the prev nt or differ from the prev nt by an m2 (a "step").  If the 
	// prev two notes are a step, new_nt must be either a step or a skip.  
	auto skip_step_rule = [&m,&midx,num_interval](const note_t& new_nt) -> bool { 
		if (midx.ch_idx < 2) { return false; }
		int ci = midx.ch_idx;
		int vi = midx.v_idx;
		//std::cout << m[vi][ci-2].print() << ", " << m[vi][ci-1].print() << ", " << new_nt.print() << std::endl;
		int prev_int = num_interval(m[vi][ci-2],m[vi][ci-1]);
		int new_int = num_interval(m[vi][ci-1],new_nt);
		if (std::abs(prev_int) >= 2) {  // Prev 2 notes are a "skip;"  2 => m3||M3
			bool tf = !(new_nt.ntl == m[vi][ci-1].ntl || std::abs(new_int)==1);
			return tf;
		} else if (std::abs(prev_int)==1) {  // Prev 2 notes are a "step;"  1 => m2||M2
			bool tf = !(std::abs(new_int)==1 || std::abs(new_int)==2);
			return tf;
		}
		return false;
	};

	// Rule 6
	// No more than one successive repeat of a note is allowed in a given voice.  Require that
	// both the oct and ntl be the same to fail the rule (return true).  
	auto rpts_gt_one = [&m,&midx](const note_t& new_nt) -> bool { 
		if (midx.ch_idx < 2) {	return false; }
		note_t pprev_nt = m[midx.v_idx][midx.ch_idx-2];
		note_t prev_nt = m[midx.v_idx][midx.ch_idx-1];
		return ((pprev_nt.ntl==new_nt.ntl && pprev_nt.oct==new_nt.oct)
			&& (prev_nt.ntl==new_nt.ntl && prev_nt.oct==new_nt.oct));
	};

	// Rule 7
	// Forbidden to repeat the highest note of a line unless said note is the tonic AND
	// the line moves in a tritone resolution or a cadence to the high tonic.  
	// At present only checks for repeats of the highest note on a given line and that
	// said repeat is of C.  
	// This rule is only defined on a completed line, since only then is it possible to decide
	// if a given repeat is a repeat of the "highest note."  Since the melody is generated one
	// chord at a time, rather than one line at a time, this rule can not be applied until the
	// entire melody is completed.  
	auto rpt_high_nt = [&m,&midx,max_frq]() -> bool { 
		if (midx.ch_idx < midx.nnts || midx.v_idx < midx.nvoices) {
			// This function can only be called on a completed melody
			return false;
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
		if (midx.v_idx == 0) { return false; }

		for (int i=0; i<midx.v_idx; ++i) {  // m.size() == nvoices
			int curr_int = std::abs(num_interval(m[i][midx.ch_idx],new_nt));
			if (curr_int == 6 || curr_int == 1) {
				return true;  // Illegal harmonic interval
			}
		}

		return false;
	};

	// Rule 9
	// For each chord, a P4 is allowed iff it does not occur between the lowest note of the
	// chord and an upper voice.  C-F is a perfect 4'th.  
	// This rule always returns false unless the working chord midx.ch_idx is complete, in which  
	// case the rule is evaluated and may return true.  
	auto harmonic_p4 = [&m,&midx,get_chord,min_frq]() -> bool {
		if (midx.v_idx < midx.nvoices) { return false; }

		bool found_c {false}; bool found_cf {false};
		note_t low_c_of_p4 {};
		std::vector<note_t> curr_chord = get_chord(midx.ch_idx);
		for (const auto& e : curr_chord) {
			if (e.ntl == ntl_t{"C"} && !found_c) {
				found_c = true;  // This is the first C we've found
				low_c_of_p4 = e;
			} else if (e.ntl == ntl_t{"C"} && found_c) {   // This is ! the first C we've found
				if (e.frq < low_c_of_p4.frq) { 
					low_c_of_p4 = e;
				}
			}

			if (e.ntl == ntl_t{"F"} && found_c) { found_cf = true; }
		}

		return found_cf && (min_frq(curr_chord) == low_c_of_p4);
		// If the chord contains a C-F, the rule is broken (return true) if the C is the
		// offending interval is the low note of the chord.  
		// This test will forbid legitimate new_nt's unless it is only made on completed chords
	};

	// Rule 10
	// Part a:  True if B-F occurs in the present chord w/o a D below the B, false otherwise.  
	// Part b:  This tritone must resolve to E-C or C-E in the next chord.  
	// TODO:  Part b not implemented
	// Always returns false if midx.v_idx indicates that the present chord is incomplete.  
	auto tritone = [&m,&midx,get_chord,ntl_ismember,ntl_lt_frq]() -> bool {
		if (midx.v_idx < midx.nvoices) { return false; }
		
		auto curr_ch = get_chord(midx.ch_idx);
		if (!(ntl_ismember(curr_ch,ntl_t{"F"}) && ntl_ismember(curr_ch,ntl_t{"B"}))) {
			return false;
		}
		
		std::sort(curr_ch.begin(),curr_ch.end(),ntl_lt_frq);
		bool found_d {false}; bool found_b {false};// bool found_f {false};
		bool found_bf {false}; bool found_db {false}; bool found_dbf {false};
		for (const auto& e : curr_ch) {
			if (e.ntl == ntl_t{"D"}) { found_d = true; }
			//if (e.ntl == ntl_t{"F"}) { found_f = true; }
			if (e.ntl == ntl_t{"B"}) { found_b = true; }
			if (e.ntl == ntl_t{"B"} && found_d) { found_db = true; }
			if (e.ntl == ntl_t{"F"} && found_b) { found_bf = true; }
			if (e.ntl == ntl_t{"F"} && found_db) { found_dbf = true; }
		}

		if (found_bf && !found_dbf) {
			return true;  // B-F is only allowed in the context of D-B-F
		}
		return false;
	};

	// Rule 12 - part a, part b
	// The second-last chord must contain the note B in exactly one voice.  In the last chord,
	// this voice must be the note C.  
	// Always returns false unless midx indicates that the second-to-last chord has been 
	// completed, at which point returns true if the rule is violated.  
	auto ch_nxtlast_contains_b = [&m,&midx,get_chord,ntl_ismember]() -> bool {
		if (midx.v_idx < midx.nvoices  // present chord midx.ch_idx is not complete
			|| midx.ch_idx != (midx.nnts-1)) {  // present chord is not the second-last
			return false;
		}
		std::vector<note_t> ch = get_chord(midx.ch_idx);
		int num_b {0};
		for (const auto& e : ch) {
			if (e.ntl == ntl_t{"B"}) { ++num_b; }
		}
		return num_b != 1;
	};
	auto lastch_contains_rsln_from_b = [&m,&midx,get_chord]() -> bool {
		if (midx.v_idx != midx.nvoices  // chord is not complete
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

	struct mel_rules {
		bool r1 {false};
		bool r2 {false};
		bool r3 {false};
		bool r4 {false};
		bool r5 {false};
		bool r6 {false};
		bool r8 {false};
	};
	struct har_rules {
		bool r9 {false};
		bool r10 {false};
		bool r12 {false};
	};

	std::vector<note_t> curr_ntpool {};
	// Position 'counters' midx.{v_idx,ch_idx} are incremented in such a way that at all  
	// locations in the loop, m[midx.v_idx][midx.ch_idx] is _not_ valid.  It's what's
	// presently being worked on; the address to which new_nt will be placed if it passes
	// all the rules.  
	bool give_up {false};
	int titer {0};
	while (!give_up) {
		while (midx.ch_idx < midx.nnts) {  // m[voice][note-num]
			while (midx.v_idx < midx.nvoices) {  // m[voice][note-num]
				++titer;
				if (titer > 500) {
					std::cout << "titer > 100" << std::endl;
				}
				//
				// Set curr_ntpool
				//
				curr_ntpool = ntpool;

				//
				// Draw potential note m[v_idx][ch_idx]
				//
				int ridx = rd(re);
				note_t new_nt = ntpool[ridx];

				//
				// Compare new_nt against the rule set
				//
				//
				mel_rules curr_mr {span_geq_oct(new_nt),
					cf_beginend_tonic(new_nt),
					noncf_beginend_tonictriad(new_nt),
					no_mel_sevenths(new_nt),
					skip_step_rule(new_nt),
					rpts_gt_one(new_nt),
					harmonic_consonant(new_nt)
				};
				if (span_geq_oct(new_nt)  // Rule 1
					|| cf_beginend_tonic(new_nt)  // Rule 2
					|| noncf_beginend_tonictriad(new_nt)  // Rule 3
					|| no_mel_sevenths(new_nt)  // Rule 4
					|| skip_step_rule(new_nt)   // Rule 5
					|| rpts_gt_one(new_nt)   // Rule 6
					|| harmonic_consonant(new_nt))  // Rule 8
				{  
					// reject new_nt; midx.v_idx not incremented
					midx.nt_rejects_curr_v += 1;
					midx.nt_rejects_curr_ch += 1;
					midx.nt_rejects_tot += 1;
					if (midx.nt_rejects_curr_v > 15 || midx.nt_rejects_curr_ch > 15) {
						std::cout << "midx.nt_rejects_curr_v > 15 || midx.nt_rejects_curr_ch > 15" << std::endl;
						midx.ch_idx = std::max(midx.ch_idx-1,0);
						midx.v_idx = 0;
						midx.nt_rejects_curr_v = 0;
						midx.nt_rejects_curr_ch = 0;
					}

				} else {
					// accept the new nt by moving to the next voice
					m[midx.v_idx][midx.ch_idx] = new_nt;
					midx.v_idx += 1;
					midx.nt_rejects_curr_v = 0;
				}
			}  // to next voice of present chord
			midx.nt_rejects_curr_ch = 0;

			har_rules curr_hr {tritone(),
				ch_nxtlast_contains_b() || lastch_contains_rsln_from_b(),
				harmonic_p4()
			};
			if (ch_nxtlast_contains_b() || lastch_contains_rsln_from_b()  // Rule 12
				|| tritone()  // Rule 10
				|| harmonic_p4())  // Rule 9
			{
				// reject the present chord
				midx.ch_idx = std::max(midx.ch_idx-1,0);
				midx.ch_rejects_curr_ch += 1;
				midx.ch_rejects_tot += 1;
				if (midx.ch_rejects_curr_ch > 15) {
					std::cout << "midx.ch_rejects_tot > 5" << std::endl;
				}
			} else {
				// accept the present chord
				midx.ch_idx += 1;
				midx.ch_rejects_curr_ch = 0;
			}

			midx.v_idx = 0;
		}  // to next ch of first voice
		
		//if (rpt_high_nt()) {  // Rule 7
		//	// discard melody
		//}
		give_up = true;
	}

	return m;
}


