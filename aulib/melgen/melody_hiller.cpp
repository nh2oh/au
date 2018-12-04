#include "randmel_gens.h"
#include "..\util\au_random.h";  // new_randeng()
#include "..\scale\spn.h";  // To compute semitone difference between nt pairs
#include "..\scale\diatonic_spn.h";
#include "..\types\ntl_t.h";
#include "..\types\frq_t.h";
#include <string>
#include <vector>
#include <iostream>
#include <exception>
#include <algorithm>
#include <random>
#include <cmath>  // std::abs()


//
// It should also be possible to implement this in the same way as melody_a(), ie, by first
// generating a completely random 2-d array of notes, scoring according to the counterpoint
// rules, mutating, re-scoring, etc...
//
// Better method:  Use the rules to set the ntpool, then any selection will work.  
//
std::vector<std::vector<note_t>> melody_hiller_ex21(const melody_hiller_params& p) {
	spn sc_cchrom {};
	diatonic_spn sc {ntl_t {"C"}, diatonic_spn::mode::major};

	auto ntstr_min_parse = parse_spn_ntstr(p.min);
	auto ntstr_max_parse = parse_spn_ntstr(p.max);
	bool min_valid = (ntstr_min_parse.is_valid && ntstr_min_parse.is_oct_set 
		&& (ntstr_min_parse.ntl == ntstr_min_parse.ntl_base));
	bool max_valid = (ntstr_max_parse.is_valid && ntstr_max_parse.is_oct_set 
		&& (ntstr_max_parse.ntl == ntstr_max_parse.ntl_base));
	if (!min_valid || !max_valid) {
		std::abort();
	}
	int scd_min = sc.to_scd(ntstr_min_parse.ntl_base,ntstr_min_parse.oct);
	int scd_max = sc.to_scd(ntstr_max_parse.ntl_base,ntstr_max_parse.oct);
	if (scd_max-scd_min < 8) {
		std::abort();
	}
	
	melody_hiller_internal::hiller21_status s {};
	s.nnts = p.nnts;
	s.nvoices = p.nvoice;
	s.rule.resize(10,0);

	std::vector<note_t> ntpool {};
	for (int s = scd_min; s<=scd_max; ++s) {
		ntpool.push_back(sc[s]);
	}

	auto re = new_randeng(true);
	std::uniform_int_distribution rd {size_t {0}, ntpool.size()-1};

	// m[voice][note]
	std::vector<std::vector<note_t>> m {};
	m.insert(m.begin(),s.nvoices,std::vector<note_t>(s.nnts,sc[0]));
	
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

	// Gets from v_idx == 0 to s.v_idx if requesting chord_idx == s.ch_idx, or from
	// v_idx == 0 to s.nvoices if chord_idx < s.ch_idx.  
	auto get_chord = [&s, &m](int chord_idx) -> std::vector<note_t> {
		int max_v_idx {0};
		if (chord_idx > s.ch_idx) {
			std::abort();
		} else if (chord_idx < s.ch_idx) {
			// Requesting a ch prior to the working ch
			max_v_idx = s.nvoices;
		} else {  // (chord_idx == s.ch_idx)
			// Requesting the working ch 
			max_v_idx = s.v_idx;
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
	// NB:  A repeat (from == to) => 1, not 0
	auto abs_staffdiff_cmn = [&sc](const note_t& from, const note_t& to) -> int {
		return std::abs(sc.to_scd(to)-sc.to_scd(from))+1;
	};
	auto abs_semidiff = [&sc_cchrom](const note_t& from, const note_t& to) -> int {
		return std::abs(sc_cchrom.to_scd(to.ntl,to.oct)-sc_cchrom.to_scd(from.ntl,from.oct));
	};

	// Rule 1
	// The max interval spanned by the lowest and highest notes on a given line must 
	// be <= 1 oct.  
	auto span_geq_oct = [&m,&s,min_frq,max_frq](const note_t& new_nt) -> bool { 
		if (s.first_ch()) { return false; }
		double ratio_lowest = (new_nt.frq)/(min_frq(m[s.v_idx]).frq);
		double ratio_highest = (max_frq(m[s.v_idx]).frq)/(new_nt.frq);
		return (ratio_lowest > 2.0 || ratio_highest > 2.0);
	};

	// Rule 2
	// cf begins+ends on the tonic
	auto cf_beginend_tonic = [&m,&s](const note_t& new_nt) -> bool { 
		if (!s.first_ch() && !s.last_ch() && !s.first_v()) {
			return false;
		}
		return new_nt.ntl != ntl_t {"C"};
	};

	// Rule 3, 11
	// Non-cf voices begin+end on tonic triad root position notes
	auto noncf_beginend_tonictriad = [&m,&s](const note_t& new_nt) -> bool { 
		if (!s.first_ch() && !s.last_ch() && s.first_v()) {
			return false;
		}

		if (s.v_idx == (s.nvoices-1)) {  // The lowest voice
			return new_nt.ntl != ntl_t {"C"};
		}
		return !(new_nt.ntl == ntl_t {"C"} || new_nt.ntl == ntl_t {"E"} 
			|| new_nt.ntl == ntl_t {"G"});
	};

	// Rule 4
	// For any voice, no intervals == m7 || M7
	auto no_mel_sevenths = [&m,&s,abs_staffdiff_cmn,abs_semidiff](const note_t& new_nt) -> bool { 
		if (s.ch_idx == 0) { return false; }
		int delta_staff = abs_staffdiff_cmn(m[s.v_idx][(s.ch_idx-1)],new_nt);
		int delta_semi = abs_semidiff(m[s.v_idx][(s.ch_idx-1)],new_nt);
		return (delta_staff == 7 && (delta_semi == 10 || delta_semi == 11));
	};

	// Rule 5
	// For the current voice, if the prev. two notes are >= an m3 apart (a "skip"), new_nt must 
	// be either a repeat of the prev nt or differ from the prev nt by an m2 (a "step").  If the 
	// prev two notes are a step, new_nt must be either a step or a skip.  
	auto skip_step_rule = [&m,&s,abs_staffdiff_cmn](const note_t& new_nt) -> bool { 
		if (s.ch_idx < 2) { return false; }
		int ci = s.ch_idx;
		int vi = s.v_idx;
		int prev_int = abs_staffdiff_cmn(m[vi][ci-2],m[vi][ci-1]);
		int new_int = abs_staffdiff_cmn(m[vi][ci-1],new_nt);
		if (prev_int >= 3) {  // Prev 2 notes are a "skip;"  3 => m3||M3
			bool tf = !(new_nt.ntl == m[vi][ci-1].ntl || new_int==2);
			return tf;
		} else if (prev_int==2) {  // Prev 2 notes are a "step;"  2 => m2||M2
			bool tf = !(new_int>=2);
			return tf;
		}
		return false;
	};

	// Rule 6
	// No more than one successive repeat of a note is allowed in a given voice.  Require that
	// both the oct and ntl be the same to fail the rule (return true).  
	auto rpts_gt_one = [&m,&s](const note_t& new_nt) -> bool { 
		if (s.ch_idx < 2) {	return false; }
		note_t pprev_nt = m[s.v_idx][s.ch_idx-2];
		note_t prev_nt = m[s.v_idx][s.ch_idx-1];
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
	auto rpt_high_nt = [&m,&s,get_chord,max_frq,ntl_lt_frq]() -> bool { 
		if (s.ch_idx < s.nnts) { return false; }  // melody not complete
		// TODO:  Condition needs to also check for completed final line...

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
	auto harmonic_consonant = [&m,&s,abs_staffdiff_cmn,abs_semidiff](const note_t& new_nt) -> bool {
		if (s.v_idx == 0) { return false; }
		for (int i=0; i<s.v_idx; ++i) {
			int staffint = abs_staffdiff_cmn(m[i][s.ch_idx],new_nt);
			int ns = abs_semidiff(m[i][s.ch_idx],new_nt);
			if (staffint==7 || staffint==2 || (staffint==4 && ns==5)) {
				return true;  //  m7 || M7 || m2 || M2 || P4
			}
		}

		return false;
	};

	// Rule 9
	// TODO:  Calc as:  (staffint==4 && ns==5)
	// For each chord, a P4 is allowed iff it does not occur between the lowest note of the
	// chord and an upper voice.  C-F is a perfect 4'th.  
	auto harmonic_p4 = [&m,&s,get_chord,min_frq](const note_t& new_nt) -> bool {
		if (s.first_v()) { return false; }

		bool found_c {false}; bool found_cf {false};
		note_t low_c_of_p4 {};
		std::vector<note_t> curr_chord = get_chord(s.ch_idx);
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
	// Always returns false if s.v_idx indicates that the present chord is incomplete.  
	auto tritone = [&m,&s,get_chord,ntl_ismember,ntl_lt_frq](const note_t& new_nt) -> bool {
		if (!s.last_v()) { return false; }
		
		auto curr_ch = get_chord(s.ch_idx);  curr_ch.push_back(new_nt);
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
	// Always returns false unless s indicates that the second-to-last chord has been 
	// completed, at which point returns true if the rule is violated.  
	auto ch_nxtlast_contains_b = [&m,&s,get_chord,ntl_ismember](const note_t& new_nt) -> bool {
		if (!s.last_v()  // present chord s.ch_idx is not complete
			|| s.ch_idx != (s.nnts-1)) {  // present chord is not the second-last
			return false;
		}  // TODO:  Does (s.nnts-1) check for second-to-last???
		std::vector<note_t> ch = get_chord(s.ch_idx);  ch.push_back(new_nt);
		int num_b {0};
		for (const auto& e : ch) {
			if (e.ntl == ntl_t{"B"}) { ++num_b; }
		}
		return num_b != 1;
	};
	auto lastch_contains_rsln_from_b = [&m,&s,get_chord](const note_t& new_nt) -> bool {
		if (!s.last_v() || !s.last_ch()) { return false; }
		std::vector<note_t> ch_nxtlast = get_chord(s.ch_idx-1);
		std::vector<note_t> ch_last = get_chord(s.ch_idx);
		ch_last.push_back(new_nt);
		for (int vi=0; vi<s.v_idx; ++vi) {
			if (ch_nxtlast[vi].ntl == ntl_t{"B"}) { 
				return ch_last[vi].ntl != ntl_t {"C"};
				// == C => rule passes => return false
			}
		}
		// Means ch_nxtlast did not contain a B - rule 12a should prevent this from
		// happening.  
		return true;
	};

	while (s.rejects_tot < p.max_rejects_tot && s.ch_idx < s.nnts) {
		// Draw potential nt to occupy m[v_idx][ch_idx]
		note_t new_nt = ntpool[rd(re)];
		s.new_nt = new_nt;

		// Comparison against the rule set
		s.set_result(1,!span_geq_oct(new_nt));
		s.set_result(2,!cf_beginend_tonic(new_nt));
		s.set_result(3,!noncf_beginend_tonictriad(new_nt));
		s.set_result(4,!no_mel_sevenths(new_nt));
		s.set_result(5,!skip_step_rule(new_nt));
		s.set_result(6,!rpts_gt_one(new_nt));
		s.set_result(7,!rpt_high_nt());
		s.set_result(8,!harmonic_consonant(new_nt));
		s.set_result(9,!harmonic_p4(new_nt));
		s.set_result(10,!tritone(new_nt));
		// Rule 3 => Rule 11
		s.set_result(12,!(ch_nxtlast_contains_b(new_nt) && lastch_contains_rsln_from_b(new_nt)));

		if (s.any_failed()) {
			if (p.debug_lvl == 2 || p.debug_lvl == 3) {
				std::cout << s.print(m) << std::endl;
			}
			if (s.rejects_curr_ch < p.rejects_regen_ch) {
				s.set_for_new_attempt_curr_nt();
			} else {
				if (p.debug_lvl == 2 || p.debug_lvl == 3) {
					std::cout << "rejects_curr_ch >= " + std::to_string(p.rejects_regen_ch) 
						+ ";  s.set_for_new_attempt_prev_ch();" << std::endl;
				}
				s.set_for_new_attempt_prev_ch();
			}
		} else {  // success
			if (p.debug_lvl == 1 || p.debug_lvl == 3) {
				std::cout << s.print(m) << std::endl;
			}
			m[s.v_idx][s.ch_idx] = new_nt;
			s.set_for_next();
		}
	}

	std::string lpout {};
	for (int ch=0; ch<p.nnts; ++ch) {
		lpout += "<";
		for (int v=0; v<p.nvoice; ++v) {
			lpout += m[v][ch].print(note_t::fmt::lp);
			if (v < (p.nvoice-1)) { lpout += " "; }
		}
		lpout += "> ";
	}
	std::cout << std::endl << lpout << std::endl;
	return m;
}


bool melody_hiller_internal::hiller21_status::first_ch() const {
	return ch_idx == 0;
}
bool melody_hiller_internal::hiller21_status::last_ch() const {
	return ch_idx == (nnts-1);
}
bool melody_hiller_internal::hiller21_status::first_v() const {
	return v_idx == 0;
}
bool melody_hiller_internal::hiller21_status::last_v() const {
	return v_idx == (nvoices-1);
}

bool melody_hiller_internal::hiller21_status::any_failed() const {
	for (const auto& e : rule) {
		if (e == -1) { return true; }
	}
	return false;
}

// Set the result of evaluating rule r;  "Rule r" => rule[r-1]
void melody_hiller_internal::hiller21_status::set_result(size_t r, bool pass) {
	if ((r-1) >= rule.size()) {
		rule.resize(r,0);  // Require size==(r+1) to store into [r], thus size==r for [r-1], 
	}
	rule[r-1] = pass ? 1 : -1;
}

// Set the result of all rules to 0
void melody_hiller_internal::hiller21_status::clear_rules() {
	for (auto e : rule) { e=0; }
}

// Called when new_nt is rejected (1 or more rules did not pass)
// Reset all rule pass/fail results & increment the curr_ch & total reject counters
void melody_hiller_internal::hiller21_status::set_for_new_attempt_curr_nt() {
	clear_rules();
	++rejects_curr_ch; ++rejects_tot;
}

// Called when new_nt is rejected (1 or more rules did not pass) and when the curr_ch reject
// counter indicates that there have been melody_hiller_params.rejects_regen_ch rejections for
// the present chord.  This begins the process of overwriting chord ch_idx-1.  
//
// Reset all rule pass/fail results, reset the curr_ch reject counter, increment the total
// total rejects counter, decrement ch_idx and set v_idx == 0
void melody_hiller_internal::hiller21_status::set_for_new_attempt_prev_ch() {
	clear_rules();
	rejects_curr_ch = 0; ++rejects_tot;
	v_idx = 0; ch_idx = std::max(0,ch_idx-1);
}

// Called when new_nt is accepted into the growing sequence.  
// Reset all rule pass/fail results & increment v_idx.
// Iff v_idx was the final voice of the ch_idx, increment ch_idx and zero the curr_ch 
// reject counter.  
void melody_hiller_internal::hiller21_status::set_for_next() {
	clear_rules();
	if (v_idx < (nvoices-1)) {  // To next v in present ch
		++v_idx;
	} else {  // To next ch
		rejects_curr_ch = 0;
		v_idx=0; ++ch_idx;
	}
}

// Prints the present sequence m, the candidate nt new_nt, and the results of the tests on
// new_nt.  
// TODO:  Not sure this needs to be a member?
std::string melody_hiller_internal::hiller21_status::print(const std::vector<std::vector<note_t>>& m) const { 
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



