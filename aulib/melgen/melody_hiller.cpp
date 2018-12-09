#include "randmel_gens.h"
#include "..\util\au_random.h"  // new_randeng()
#include "..\scale\spn.h"  // To compute semitone difference between nt pairs
#include "..\scale\diatonic_spn.h"
#include "..\types\ntl_t.h"
#include "..\types\frq_t.h"
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
// Better method:  Use the rules to set the ntpool, then any random note selection will work.  
//
// 
std::vector<std::vector<note_t>> melody_hiller_ex21(const melody_hiller_params& p) {
	using namespace melody_hiller_internal;
	//
	// Melody accumulated in m; status object s
	// Voice 0 (m[0][ch], s.v_idx==0) is the cf
	//
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

	// m[voice][note] contains the melody
	// Preallocation with all elements == C(0), probably outside of the range specified by
	// the caller (though i do not rely on this assumption).  
	std::vector<std::vector<note_t>> m {};
	m.insert(m.begin(),s.nvoices,std::vector<note_t>(s.nnts,sc[0]));
	

	// Used for std::find(std::vector<note_t>) to find a particular ntl
	auto ntl_eq = [](const note_t& lhs, const note_t& rhs) -> bool {
		return lhs.ntl == rhs.ntl;
	};
	// Used for std::find(std::vector<note_t>) to find a particular ntlo
	auto ntlo_eq = [](const note_t& lhs, const note_t& rhs) -> bool {
		return lhs.ntl == rhs.ntl && lhs.oct == rhs.oct;
	};
	// Used for std::sort(std::vector<note_t>) to sort by frq
	auto ntl_lt_frq = [](const note_t& lhs, const note_t& rhs) -> bool {
		return lhs.frq < rhs.frq;
	};
	auto ntl_ismember = [](const std::vector<note_t>& nts, const ntl_t& nt) -> bool {
		for (int i=0; i<nts.size(); ++i) {
			if (nts[i].ntl==nt) { return true; }
		}
		return false;
	};





	// Number of staff positions between two notes 
	// 1 => m2 || M2; 2 => m3 || M3;  ...
	auto n_staff = [&sc](const note_t& from, const note_t& to) -> int {
		return sc.to_scd(to)-sc.to_scd(from);
	};
	// Number of semitones between two notes 
	auto n_semi = [&sc_cchrom](const note_t& from, const note_t& to) -> int {
		return sc_cchrom.to_scd(to.ntl,to.oct)-sc_cchrom.to_scd(from.ntl,from.oct);
	};
	// Absolute value of the number of staff positions between two notes PLUS ONE.  
	// Thus for C(3)->D(3) returns 2; C(3)->C(3) returns 1.  Never returns 0.  
	// "cmn" => "common"
	auto abs_staffdiff_cmn = [&sc](const note_t& from, const note_t& to) -> int {
		return std::abs(sc.to_scd(to)-sc.to_scd(from))+1;
	};
	// Absolute value of the number of semitones between two notes 
	auto abs_semidiff = [&sc_cchrom](const note_t& from, const note_t& to) -> int {
		return std::abs(sc_cchrom.to_scd(to.ntl,to.oct)-sc_cchrom.to_scd(from.ntl,from.oct));
	};



	// Rule 2
	// cf begins+ends on the tonic
	auto cf_beginend_tonic = [&m,&s](const note_t& new_nt) -> bool { 
		if (!s.first_v() || !(s.first_ch() || s.last_ch())) {
			return false;  // non-cf and/or internal chord
		}
		return new_nt.ntl != ntl_t {"C"};
	};

	// Rule 3
	// Non-cf voices begin+end on tonic triad root position notes
	auto noncf_beginend_tonictriad = [&m,&s](const note_t& new_nt) -> bool { 
		if (s.first_v() || !(s.first_ch() || s.last_ch())) {
			return false;  // cf and/or internal chord
		}

		if (s.v_idx == (s.nvoices-1)) {  // The lowest voice
			return new_nt.ntl != ntl_t {"C"};
		}
		return !(new_nt.ntl == ntl_t {"C"} || new_nt.ntl == ntl_t {"E"} 
			|| new_nt.ntl == ntl_t {"G"});
	};

	// Rule 4
	// No melodic intervals == m7 || M7
	auto no_mel_sevenths = [&m,&s,abs_staffdiff_cmn,abs_semidiff](const note_t& new_nt) -> bool { 
		if (s.first_ch()) { return false; }
		int delta_staff = abs_staffdiff_cmn(m[s.v_idx][(s.ch_idx-1)],new_nt);
		int delta_semi = abs_semidiff(m[s.v_idx][(s.ch_idx-1)],new_nt);
		return (delta_staff == 7 && (delta_semi == 10 || delta_semi == 11));
	};

	// Rule 5
	// For the current voice, if the prev. two notes are >= an m3 apart (a "skip"), 
	// new_nt must be either a repeat of the prev nt or differ from the prev nt by 
	// an m2||M2 (a "step").  If the prev two notes are a "step," new_nt must be 
	// either a step or a skip.  It follows that the only way to break out of this
	// skip-step || skip-rpt || step-skip || step-step condition is for the previous 
	// interval to be a repeat.  
	auto skip_step_rule = [&m,&s,abs_staffdiff_cmn](const note_t& new_nt) -> bool { 
		if (s.ch_idx < 2) { return false; }
		note_t prev_nt = m[s.v_idx][s.ch_idx-1];
		note_t pprev_nt = m[s.v_idx][s.ch_idx-2];
		int prev_int = abs_staffdiff_cmn(pprev_nt,prev_nt);
		int new_int = abs_staffdiff_cmn(prev_nt,new_nt);
		if (prev_int >= 3) {  // Prev 2 notes are a "skip;"  3 => m3||M3
			bool tf = !(new_int==1 || new_int==2);  // NB 1 => ntl's are ==
			return tf;
		} else if (prev_int==2) {  // Prev 2 notes are a "step;"  2 => m2||M2
			bool tf = !(new_int>=2);
			return tf;
		}
		return false;
	};

	// Rule 6
	// No more than one successive repeat of a note is allowed in a given voice.  Require 
	// that both the oct and ntl be the same to fail the rule (return true).  
	auto rpts_gt_one = [&m,&s](const note_t& new_nt) -> bool { 
		if (s.ch_idx < 2) {	return false; }
		note_t pprev_nt = m[s.v_idx][s.ch_idx-2];
		note_t prev_nt = m[s.v_idx][s.ch_idx-1];
		return ((pprev_nt.ntl==new_nt.ntl && pprev_nt.oct==new_nt.oct)
			&& (prev_nt.ntl==new_nt.ntl && prev_nt.oct==new_nt.oct));
	};

	// Rule 7
	// Forbidden to repeat the highest note of a line unless the repeat is of the tonic
	// and (1) or (2):
	// 1)  the repeat resolves a preceeding melodic tritone
	// 2)  the repeat involves a cadence to the high tonic: Hence the full sequence is:
	//     C(i)-E(i)-G(i)-C(i+1)-C(i+1)
	//
	// This rule is only defined on a completed line, since only then is it possible to 
	// decide f a given repeat is a repeat of the "highest note of the line."  Returns 
	// false until the working note is the final note of the line.  
	auto rpt_high_nt = [&m,&s](const note_t& new_nt) -> bool { 
		if (!s.last_ch()) { return false; }  // line is not complete
		
		std::vector<note_t> curr_voice = m[s.v_idx];  curr_voice.push_back(new_nt);
		note_t max_nt = max_frq(curr_voice);
		for (int i=1; i<curr_voice.size(); ++i) {  // i ~ current nt of voice
			if (!(curr_voice[i-1] == max_nt && curr_voice[i] == max_nt)) {
				continue;  // spare some indentation below
			}
			// max_nt is repeated at chords i-1->i
			if (max_nt.ntl != ntl_t {"C"}) { 
				return true;  // The i-1->i repeat is not the tonic
			}
			// The i-1->i repeat is the tonic
			// Now must check that the repeat involves a tritone resolution or
			// a cadence to the high tonic.  
			if (i < 3) { return true; }
			std::vector<note_t> pre_rpt_mel {curr_voice[i-3],curr_voice[i-2]};
			if (!ch_contains_tritone(pre_rpt_mel)) { return true; }

			// Now check for cadence to high tonic.  
			if (i < 4) { return true; }
			pre_rpt_mel = {curr_voice[i-4],curr_voice[i-3],curr_voice[i-2]};
			bool is_cad_high_tonic = (curr_voice[i-4].ntl == ntl_t{"C"}
				&& curr_voice[i-3].ntl == ntl_t{"E"}
				&& curr_voice[i-2].ntl == ntl_t{"G"}
				&& curr_voice[i-4].frq < curr_voice[i-3].frq
				&& curr_voice[i-3].frq < curr_voice[i-2].frq
				&& curr_voice[i-2].frq < curr_voice[i-1].frq); // i-1 => first nt of rpt
			if (!is_cad_high_tonic) { return true; }
		}  // to next nt i of curr_voice
		return false;
	};

	// Rule 8
	// For each chord, only "consonant" harmonic intervals are permitted (includes, but
	// not limited to: U, m3, M3, P5, 6'ths, O).  
	// Forbid:  m2, M2, m7, M7
	// The tritone is dissonant, but is allowed in some cases; see rule 10
	auto harmonic_consonant = [&m,&s,abs_staffdiff_cmn,abs_semidiff]
								(const note_t& new_nt) -> bool {
		if (s.first_v()) { return false; }
		
		std::vector<note_t> curr_ch = get_chord(s,m,s.ch_idx);
		for (const auto& e : curr_ch) {
			int staffint = abs_staffdiff_cmn(e,new_nt);
			int ns = abs_semidiff(e,new_nt);
			if (staffint==7 || staffint==2) {
				return true;  //  m7 || M7 || m2 || M2
			}
		}
		//curr_ch.push_back(new_nt);
		//if (ch_contains_tritone(curr_ch)) { return true; } Let rule 10 enforce
		return false;
	};

	// Rule 9
	// For each chord, a P4 is allowed iff it does not occur between the lowest 
	// note of the chord and an upper voice.  
	// Returns false until s.v_idx is the last voice of the chord; only then is 
	// it possible to define "lowest note of the chord."
	auto harmonic_p4 = [&m,&s,abs_staffdiff_cmn,
						abs_semidiff,ntlo_eq](const note_t& new_nt) -> bool {
		if (!s.last_v()) { return false; }

		std::vector<note_t> curr_ch = get_chord(s,m,s.ch_idx);
		note_t lowest_nt = min_frq(curr_ch);
		for (const auto& e : curr_ch) {
			int staffint = abs_staffdiff_cmn(e,new_nt);
			int ns = abs_semidiff(e,new_nt);
			if (!(staffint==4 && ns==5)) { continue; }  // Not a p4

			// e->new_nt _is_ a P4; is either e or new_nt the lowest nt of the chord?
			if (ntlo_eq(lowest_nt,e) || ntlo_eq(lowest_nt,new_nt)) {
				return true;
			}
		}
		return false;
	};

	// Rule 10
	// Part a:  True if a tritone (B(i)->F(i+1) in Cmaj) occurs in the present chord w/o 
	// a D below the B; false otherwise.  
	// Part b:  This tritone must resolve to E-C or C-E in the next chord.  
	// TODO:  Part b not implemented.  
	// Always returns false if s.v_idx indicates that the present chord is incomplete, since
	// only then is it possible to know if a D occurs below the B involved in the tritone.  
	auto d_below_tritone = [&m,&s,
							ntl_lt_frq](const note_t& new_nt) -> bool {
		if (!s.last_v()) { return false; }
		
		auto curr_ch = get_chord(s,m,s.ch_idx);  curr_ch.push_back(new_nt);
		if (!ch_contains_tritone(curr_ch)) { return false; }
		
		// Build test_ch, containing all notes from curr_ch except B notes above 
		// the lowest D note.  
		std::sort(curr_ch.begin(),curr_ch.end(),ntl_lt_frq);
		bool found_d {false};  std::vector<note_t> test_ch {};
		for (const auto& e : curr_ch) {
			if (e.ntl == ntl_t{"D"}) { found_d = true; }
			if (found_d && e.ntl == ntl_t{"B"}) {
				// Once the first D has been found, any B note must be higher;
				// do not add any B above the lowest D to test_ch.  
				continue;
			}
			test_ch.push_back(e);
		}
		// If the test_ch still contains a tritone, it can only mean that the tritone 
		// involves a B below the lowest D, since all B's above the lowest D were removed.
		// Rule 10 is violated if the tritone does not have a D at its "root."
		return ch_contains_tritone(test_ch);
	};

	// Rule 11
	// The lowest note in the first and last chords must be the tonic
	auto beginend_tonictriad_rootpos = [&m,&s](const note_t& new_nt) -> bool { 
		if (!s.last_v() || !(s.first_ch() || s.last_ch())) {
			return false;  // curr pos != last-voice and/or curr pos == an internal chord
		}
		auto curr_ch = get_chord(s,m,s.ch_idx);  curr_ch.push_back(new_nt);
		return (min_frq(curr_ch).ntl != ntl_t{"C"});
	};

	// Rule 12 - part a, part b
	// The second-last chord must contain the note B in exactly one voice.  In the last chord,
	// this voice must be the note C.  
	// Always returns false if the working note is not the final note of the second-to-last
	// chord.  
	auto ch_nxtlast_contains_b = [&m,&s,ntl_ismember](const note_t& new_nt) -> bool {
		if (!s.last_v() || s.ch_idx != (s.nnts-2)) { 
			return false;
		}
		std::vector<note_t> ch = get_chord(s,m,s.ch_idx);  ch.push_back(new_nt);
		int num_b {0};
		for (const auto& e : ch) {
			if (e.ntl == ntl_t{"B"}) { ++num_b; }
		}
		return num_b != 1;
	};
	auto lastch_contains_rsln_from_b = [&m,&s](const note_t& new_nt) -> bool {
		if (!s.last_v() || !s.last_ch()) { return false; }
		std::vector<note_t> ch_nxtlast = get_chord(s,m,s.ch_idx-1);
		std::vector<note_t> ch_last = get_chord(s,m,s.ch_idx); ch_last.push_back(new_nt);
		for (int i=0; i<ch_nxtlast.size(); ++i) {  // ch_nxtlast.size() == ch_last.size()
			if (ch_nxtlast[i].ntl == ntl_t{"B"}) { 
				return ch_last[i].ntl != ntl_t {"C"};
				// == C => rule passes => return false
			}
		}
		// Means ch_nxtlast did not contain a B; rule 12a should prevent this
		return true;
	};

	// Begin melody generation
	// Each iteration of the loop is responsible for adding a single note new_nt chosen
	// randomly from ntpool.  The target location for new_nt in the melody, managed by the
	// status object s, is m[s.v_idx][s.ch_idx].  Note that s does not contain, and does 
	// not ever mutate m; adding new_nt to m (if it passes all tests) is done manually in
	// the loop.  The status object s effects rejecting notes and rewriting pervious portions
	// of the melody as needed by setting the position indicies s.v_idx, s.ch_idx:
	// s.set_for_next(); advances v_idx,ch_idx by one note
	// s.set_for_new_attempt_prev_ch(); sets v_idx==0,ch_idx==ch_idx-1
	// s.set_for_new_attempt_mel(); sets v_idx==0,ch_idx==ch_idx==0
	// In the latter two cases the "rejected" notes in m, which on subsequent iterations 
	// will be rewritten, are not cleared in any way.  If melody generation stops 
	// prematurely for some reason, there may be notes in m ahead of m[s.v_idx][s.ch_idx]
	// not == to the value m was initialized with.  
	// The decision of which s.set_for_*() function to call is made by the loop, not the status
	// object.  This is because settings like the maximum number of total attempts etc live
	// in parameters passes in by the caller (melody_hiller_params p) and are not part of the
	// "status" per se of the process.  Further, this makes the status object useful in other
	// hiller-like functions with different rules, or with situations in which certain rules 
	// apply only on selected iterations or in combination with other rules.  I think that 
	// the "status" object should just keep track of the status, not be responsible for 
	// building m; this is the job of the algorithm writer.  
	// 
	//
	int total_melody_resets {0};
	while (s.rejects_tot < p.max_rejects_tot && s.ch_idx < s.nnts) {
		// Draw potential nt to occupy m[v_idx][ch_idx]
		note_t new_nt = ntpool[rd(re)];
		s.new_nt = new_nt;

		// Comparison against the rule set
		s.set_result(1,!line_spans_gt_oct(s,m,new_nt));
		s.set_result(2,!cf_beginend_tonic(new_nt));
		s.set_result(3,!noncf_beginend_tonictriad(new_nt));
		s.set_result(4,!no_mel_sevenths(new_nt));
		s.set_result(5,!skip_step_rule(new_nt));
		s.set_result(6,!rpts_gt_one(new_nt));
		s.set_result(7,!rpt_high_nt(new_nt));
		s.set_result(8,!harmonic_consonant(new_nt));
		s.set_result(9,!harmonic_p4(new_nt));
		s.set_result(10,!d_below_tritone(new_nt));
		s.set_result(11,!beginend_tonictriad_rootpos(new_nt));
		s.set_result(12,!(ch_nxtlast_contains_b(new_nt) || lastch_contains_rsln_from_b(new_nt)));

		if (s.any_failed()) {
			if (p.debug_lvl == 3 || p.debug_lvl == 4) {
				std::cout << s.print(m) << std::endl;
			}

			if (s.rejects_tot > 0 && s.rejects_tot%((p.rejects_regen_ch)*(p.nnts))==0) {
				if (p.debug_lvl == 3 || p.debug_lvl == 4) {
					std::cout << "For the current mel, tot rejects >= " 
						<< std::to_string((p.rejects_regen_ch)*(p.nnts)) 
						<< ";  s.set_for_new_attempt_mel();" << std::endl;
				}
				s.set_for_new_attempt_mel();
				++total_melody_resets;
			} else if (s.rejects_curr_ch < p.rejects_regen_ch) {
				s.set_for_new_attempt_curr_nt();
			} else {
				if (p.debug_lvl == 3 || p.debug_lvl == 4) {
					std::cout << "rejects_curr_ch >= " + std::to_string(p.rejects_regen_ch) 
						+ ";  s.set_for_new_attempt_prev_ch();" << std::endl;
				}
				s.set_for_new_attempt_prev_ch();
			}
		} else {  // Success:  Go to next note
			if (p.debug_lvl >= 2) {
				std::cout << s.print(m) << std::endl;
			}
			m[s.v_idx][s.ch_idx] = new_nt;
			s.set_for_next();
		}
	}
	
	if (p.debug_lvl >= 1) {
		if (s.rejects_tot >= p.max_rejects_tot) {
			std::cout << "Failed to generate melody; " << std::to_string(s.rejects_tot)
				<< " total rejected iterations exceeded the maximum of "
				<< std::to_string(p.max_rejects_tot) 
				<< ".  \nHere is how far the process got (including junk notes at the end)\n";
		} else {
			std::cout << "\nSuccess!  " << std::to_string(s.rejects_tot) << " total note-rejections\n";
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
	}

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


// Called when new_nt is rejected (1 or more rules did not pass) and when the total rejects
// counter indicates that there have been a multiple of:
// melody_hiller_params.rejects_regen_ch * melody_hiller_params.nnts rejections.  
//
// Reset all rule pass/fail results, reset the curr_ch reject counter, increment the total
// total rejects counter, set ch_idx == 0 and set v_idx == 0
void melody_hiller_internal::hiller21_status::set_for_new_attempt_mel() {
	clear_rules();
	rejects_curr_ch = 0; ++rejects_tot;
	v_idx = 0; ch_idx = 0;
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


namespace melody_hiller_internal {

note_t min_frq (const std::vector<note_t>& nts) {
	return *std::min_element(nts.begin(),nts.end(),
		[](const note_t& lhs,const note_t& rhs){return rhs.frq<rhs.frq;});
};
note_t max_frq (const std::vector<note_t>& nts) {
	return *std::max_element(nts.begin(),nts.end(),
		[](const note_t& lhs,const note_t& rhs){return rhs.frq<rhs.frq;});
};

std::vector<note_t> get_chord(const hiller21_status& s, const hiller_melody& m, const int chord_idx) {
	int max_v_idx{0};
	if(chord_idx > s.ch_idx) {
		std::abort();
	} else if(chord_idx < s.ch_idx) { // Requesting a ch prior to the working ch
		max_v_idx = s.nvoices;
	} else {  // Requesting the working ch (chord_idx == s.ch_idx)
		max_v_idx = s.v_idx;
	}

	std::vector<note_t> ch {};
	for(int i=0; i<max_v_idx; ++i) {
		ch.push_back(m[i][chord_idx]);  // m[voice][note]
	}
	return ch;
}

// Get notes from ch 0 up to and including the current ch, but, for the urrent ch,
// only get the note if the voice being requested v_idx is < s.v_idx, ie, if that note has
// already been generated.  
// Assumes voices are generated starting from 0.
std::vector<note_t> get_voice(const hiller21_status& s, const hiller_melody& m, const int v_idx) {
	std::vector<note_t> voice {};
	for (int i=0; i<=s.ch_idx; ++i) {
		if (v_idx >= s.v_idx && i==s.ch_idx) { break; }
		voice.push_back(m[v_idx][i]);
	}
	return voice;
}

// True iff the input note_t vector contains the notes F(i) and B(i).  This is the
// only tritone in the Cmaj scale.  Alternatively, one could also consider the 
// interval B(i) -> F(i+1) (also 6 semitones) to be a tritone, but i don't think 
// Hiller does.  
bool ch_contains_tritone (const std::vector<note_t>& ch) {
	if(ch.size() < 2) { return false; }

	note_t nt_F {ntl_t{"F"},octn_t{0},frq_t{1}};
	for(const auto& e : ch) {
		if (e.ntl != ntl_t {"B"}) { continue; }
		
		// e is a B-note
		nt_F.oct = e.oct;
		auto it = std::find_if(ch.begin(),ch.end(),
			[&nt_F](const note_t& nt){ return nt.ntl==nt_F.ntl && nt.oct==nt_F.oct;});
		if(it!=ch.end()) { return true; }
	}
	return false;
}

// Rule 1
// The max interval spanned by the lowest and highest notes on a given line must 
// be <= 1 oct.  
bool line_spans_gt_oct(const hiller21_status& s, const hiller_melody& m, const note_t& new_nt) {
	if (s.first_ch()) { return false; }
	auto curr_v = get_voice(s,m,s.v_idx);
	double ratio_lowest = (new_nt.frq)/(min_frq(curr_v).frq);
	double ratio_highest = (max_frq(curr_v).frq)/(new_nt.frq);
	return (ratio_lowest > 2.0 || ratio_highest > 2.0);
};







}  // namespace melody_hiller_internal




