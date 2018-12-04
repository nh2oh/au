#include "randmel_gens.h"
#include "..\util\au_random.h";
#include "..\scale\spn.h";
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
	spn sc_cchrom {};
	diatonic_spn sc {ntl_t {"C"}, diatonic_spn::mode::major};
	int scd_min = sc.to_scd(ntl_t{"C"},octn_t{3});
	int scd_max = sc.to_scd(ntl_t{"C"},octn_t{5});

	struct m_status {
		int nnts {12};  // TODO:   Not the best name... nchs?
		int nvoices {3};

		int ch_idx {0};  // TODO:  vi,ci ??  v,c ??
		int v_idx {0};
		note_t new_nt {};
		int rejects_curr_ch {0};
		int rejects_tot {0};
		std::array<int,12> rule {0};  // -1=>fail, 0=>ne, 1=>pass

		void set_result(size_t r, bool pass) {
			rule[r-1] = pass ? 1 : -1;
		}

		bool any_failed() const {
			for (const auto& e : rule) {
				if (e == -1) { return true; }
			}
			return false;
		}
		void set_for_new_attempt_curr_nt() {
			rule.fill(0);
			++rejects_curr_ch; ++rejects_tot;
		}
		void set_for_next() {
			rule.fill(0);
			if (v_idx < (nvoices-1)) {  // To next v in present ch
				++v_idx;
			} else {  // To next ch
				rejects_curr_ch = 0;
				v_idx=0; ++ch_idx;
			}
		}
		void set_for_new_attempt_prev_ch() {
			rule.fill(0);
			rejects_curr_ch = 0; ++rejects_tot;
			v_idx = 0; ch_idx = std::max(0,ch_idx-1);
		}
		
		std::string print(const std::vector<std::vector<note_t>>& m) const { 
			std::string s {};

			for (int v=0; v<nvoices; ++v) {  // v<nvoices => always print 2 rows
				s += std::to_string(v) + ":  ";
				for (int ch=0; ch<=ch_idx; ++ch) {  // NB:  leq! ch<=ch_idx
					if (ch==ch_idx && v >= v_idx) { continue; }
					s += m[v][ch].print() + "  ";
				}
				if (v == v_idx) { s+= " *  <-" + new_nt.print(); }
				s += "\n";
			}

			std::string failstr {}; std::string passstr {}; std::string nestr {};
			for (int i=0; i<rule.size(); ++i) {
				if (rule[i] == -1) {
					if (failstr.size() > 0) { failstr += ", "; }  // add sep iff have found prior
					failstr += std::to_string(i+1);
				} else if (rule[i] == 0) {
					if (nestr.size() > 0) { nestr += ", "; }  // add sep iff have found prior
					nestr += std::to_string(i+1);
				} else if (rule[i] == 1) {
					if (passstr.size() > 0) { passstr += ", "; }  // add sep iff have found prior
					passstr += std::to_string(i+1);
				}
			}
			s += "Passed:  " + passstr + "\n" + "Failed:  " + failstr + "\n" + "NE:  " + nestr + "\n";
			s += failstr.size()>0 ? "Overall Failed " : "Overall Passed ";
			s += "(" + std::to_string(ch_idx) + "," + std::to_string(v_idx) + ")\n";
			return s;
		}
	};
	m_status midx {};  // Current position in the developing m

	std::vector<note_t> ntpool {};
	for (int s = scd_min; s<=scd_max; ++s) {
		ntpool.push_back(sc[s]);
	}

	auto re = new_randeng(true);
	std::uniform_int_distribution rd {size_t {0}, ntpool.size()-1};

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
	auto n_staff = [&sc](const note_t& from, const note_t& to) -> int {
		return sc.to_scd(to)-sc.to_scd(from);
	};
	auto n_semi = [&sc_cchrom](const note_t& from, const note_t& to) -> int {
		return sc_cchrom.to_scd(to.ntl,to.oct)-sc_cchrom.to_scd(from.ntl,from.oct);
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
	auto no_mel_sevenths = [&m,&midx,n_staff,n_semi](const note_t& new_nt) -> bool { 
		if (midx.ch_idx == 0) { return false; }
		int delta_staff = std::abs(n_staff(m[midx.v_idx][(midx.ch_idx-1)],new_nt));
		int delta_semi = std::abs(n_semi(m[midx.v_idx][(midx.ch_idx-1)],new_nt));
		return delta_staff == 6 && (delta_semi == 10 || delta_semi == 11);
	};

	// Rule 5
	// For the current voice, if the prev. two notes are >= an m3 apart (a "skip"), new_nt must 
	// be either a repeat of the prev nt or differ from the prev nt by an m2 (a "step").  If the 
	// prev two notes are a step, new_nt must be either a step or a skip.  
	auto skip_step_rule = [&m,&midx,n_staff](const note_t& new_nt) -> bool { 
		if (midx.ch_idx < 2) { return false; }
		int ci = midx.ch_idx;
		int vi = midx.v_idx;
		//std::cout << m[vi][ci-2].print() << ", " << m[vi][ci-1].print() << ", " << new_nt.print() << std::endl;
		int prev_int = n_staff(m[vi][ci-2],m[vi][ci-1]);
		int new_int = n_staff(m[vi][ci-1],new_nt);
		if (std::abs(prev_int) >= 2) {  // Prev 2 notes are a "skip;"  2 => m3||M3
			bool tf = !(new_nt.ntl == m[vi][ci-1].ntl || std::abs(new_int)==1);
			return tf;
		} else if (std::abs(prev_int)==1) {  // Prev 2 notes are a "step;"  1 => m2||M2
			//bool tf = !(std::abs(new_int)==1 || std::abs(new_int)==2);
			bool tf = !(std::abs(new_int)>=1);
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
	// Forbidden to repeat the highest note of a line unless, for the second note n of the repeat:
	// n is the tonic AND (the repeat involves a tritone resolution or a cadence to the high tonic).  
	// At present does not yet verify the cad. to high tonic.  
	// This rule is only defined on a completed line, since only then is it possible to decide
	// if a given repeat is a repeat of the "highest note of the line."  Since the melody is generated
	// one chord at a time rather than one line at a time, this rule returns false until the final note
	// of each line is being evaluated.  
	auto rpt_high_nt = [&m,&midx,get_chord,max_frq,ntl_lt_frq]() -> bool { 
		if (midx.ch_idx < midx.nnts) { return false; }  // melody not complete

		for (int i=0; i<m.size(); ++i) {  // i ~ current voice
			note_t max_nt = max_frq(m[i]);  // Highest nt of voice i
			for (int j=0; j<(m[i].size()-1); ++j) {  // j ~ current nt of voice i
				if (m[i][j] == max_nt && m[i][j+1] == max_nt) {
					if (max_nt.ntl != ntl_t {"C"} || j == 0) { return true; }
					// Now must check that the repeat involves a tritone resolution or
					// a cadence to the high tonic.  

					// Check for tritone res
					bool tritone_res {false};
					auto prev_ch = get_chord(j-1); std::sort(prev_ch.begin(),prev_ch.end(),ntl_lt_frq);
					bool found_b {false};
					for (const auto& e : prev_ch) {
						if (e.ntl == ntl_t {"B"} && !found_b) {
							found_b == true;  // The first B that is found will be the lowest B
						} else if (e.ntl == ntl_t {"F"} && found_b) {
							tritone_res = true;  // Any F after a B is higher than that B
						}
					}
					if (tritone_res) { return false; }

					// Check for cadence to high tonic.  
					return true;
				}
			}  // to next nt j of voice i
		}  // to next voice i
		return false;
	};

	// Rule 8
	// For each chord, the only harmonic intervals permitted are U, m3, M3, P5, 6'ths, O.  
	// Forbidden are m2, M2, m7, M7, tritone
	// TODO:  How to include tritone?
	auto harmonic_consonant = [&m,&midx,n_staff,n_semi](const note_t& new_nt) -> bool {
		if (midx.v_idx == 0) { return false; }

		for (int i=0; i<midx.v_idx; ++i) {  // m.size() == nvoices
			int curr_int = std::abs(n_staff(m[i][midx.ch_idx],new_nt));
			int curr_ns = std::abs(n_semi(m[i][midx.ch_idx],new_nt));
			if (curr_int == 6 || curr_int == 1) {
				return true;  // m2 || M2 || m7 || M7
			}
			if (curr_int == 3 && curr_ns == 5) {
				auto conflict_nt = m[i][midx.ch_idx];
				return true;  // P4
			}
		}

		return false;
	};

	// Rule 9
	// For each chord, a P4 is allowed iff it does not occur between the lowest note of the
	// chord and an upper voice.  C-F is a perfect 4'th.  
	auto harmonic_p4 = [&m,&midx,get_chord,min_frq](const note_t& new_nt) -> bool {
		if (midx.v_idx ==0) { return false; }

		bool found_c {false}; bool found_cf {false};
		note_t low_c_of_p4 {};
		std::vector<note_t> curr_chord = get_chord(midx.ch_idx);
		curr_chord.push_back(new_nt);
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
	auto tritone = [&m,&midx,get_chord,ntl_ismember,ntl_lt_frq](const note_t& new_nt) -> bool {
		if (midx.v_idx < (midx.nvoices-1)) { return false; }
		
		auto curr_ch = get_chord(midx.ch_idx);  curr_ch.push_back(new_nt);
		if (!(ntl_ismember(curr_ch,ntl_t{"F"}) && ntl_ismember(curr_ch,ntl_t{"B"}))) {
			return false;
		}
		
		std::sort(curr_ch.begin(),curr_ch.end(),ntl_lt_frq);
		bool found_d {false}; bool found_b {false};
		bool found_bf {false}; bool found_db {false}; bool found_dbf {false};
		for (const auto& e : curr_ch) {
			if (e.ntl == ntl_t{"D"}) { found_d = true; }
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
	auto ch_nxtlast_contains_b = [&m,&midx,get_chord,ntl_ismember](const note_t& new_nt) -> bool {
		if (midx.v_idx < (midx.nvoices-1)  // present chord midx.ch_idx is not complete
			|| midx.ch_idx != (midx.nnts-1)) {  // present chord is not the second-last
			return false;
		}
		std::vector<note_t> ch = get_chord(midx.ch_idx);  ch.push_back(new_nt);
		int num_b {0};
		for (const auto& e : ch) {
			if (e.ntl == ntl_t{"B"}) { ++num_b; }
		}
		return num_b != 1;
	};
	auto lastch_contains_rsln_from_b = [&m,&midx,get_chord](const note_t& new_nt) -> bool {
		if (midx.v_idx < (midx.nvoices-1)  // chord is not complete
			|| midx.ch_idx != midx.nnts) { return false; }
		std::vector<note_t> ch_nxtlast = get_chord(midx.ch_idx-1);
		std::vector<note_t> ch_last = get_chord(midx.ch_idx);
		ch_last.push_back(new_nt);
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

	while (midx.rejects_tot < 500 && midx.ch_idx < midx.nnts) {
		// Draw potential nt to occupy m[v_idx][ch_idx]
		note_t new_nt = ntpool[rd(re)];
		midx.new_nt = new_nt;

		// Comparison against the rule set
		midx.set_result(1,!span_geq_oct(new_nt));
		midx.set_result(2,!cf_beginend_tonic(new_nt));
		midx.set_result(3,!noncf_beginend_tonictriad(new_nt));
		midx.set_result(4,!no_mel_sevenths(new_nt));
		midx.set_result(5,!skip_step_rule(new_nt));
		midx.set_result(6,!rpts_gt_one(new_nt));
		midx.set_result(7,!rpt_high_nt());
		midx.set_result(8,!harmonic_consonant(new_nt));
		midx.set_result(9,!harmonic_p4(new_nt));
		midx.set_result(10,!tritone(new_nt));
		midx.set_result(12,!(ch_nxtlast_contains_b(new_nt) && lastch_contains_rsln_from_b(new_nt)));
		
		std::cout << midx.print(m) << std::endl;
		if (midx.any_failed() && midx.rejects_curr_ch < 20) {
			midx.set_for_new_attempt_curr_nt();
		} else if (midx.any_failed() && midx.rejects_curr_ch >= 20) {
			std::cout << "rejects_curr_ch >= 20;  midx.set_for_new_attempt_prev_ch();" << std::endl;
			midx.set_for_new_attempt_prev_ch();
		} else {  // success
			m[midx.v_idx][midx.ch_idx] = new_nt;
			midx.set_for_next();
		}
	}

	return m;
}


