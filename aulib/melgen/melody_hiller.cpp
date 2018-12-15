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
// More than one of the rules relies implictly on the asumption that voice 0 is the cf.
// More than one of the rules relies implictly on the asumption that notes are added to
// m from low->high idx.
// 
// TODO:  Rule funcs can use s.new_nt
// 
std::vector<std::vector<note_t>> melody_hiller_ex21(const melody_hiller_params& p) {
	using namespace melody_hiller_internal;

	diatonic_spn cmaj {ntl_t {"C"}, diatonic_spn::mode::major};
	auto ntstr_min_parse = parse_spn_ntstr(p.min);
	auto ntstr_max_parse = parse_spn_ntstr(p.max);
	bool min_valid = (ntstr_min_parse.is_valid && ntstr_min_parse.is_oct_set 
		&& (ntstr_min_parse.ntl == ntstr_min_parse.ntl_base));
	bool max_valid = (ntstr_max_parse.is_valid && ntstr_max_parse.is_oct_set 
		&& (ntstr_max_parse.ntl == ntstr_max_parse.ntl_base));
	if (!min_valid || !max_valid) {
		std::abort();
	}

	// Note pool; range must span at least 1 octave
	std::vector<note_t> ntpool {};
	int scd_min = cmaj.to_scd(ntstr_min_parse.ntl_base,ntstr_min_parse.oct);
	int scd_max = cmaj.to_scd(ntstr_max_parse.ntl_base,ntstr_max_parse.oct);
	if (scd_max-scd_min < 8) { std::abort(); }
	for (int scd=scd_min; scd<=scd_max; ++scd) {
		ntpool.push_back(cmaj[scd]);
	}

	// TODO:  No need to manually fill; can call reserve().  Then can use clear()
	// to reset the melody.  
	// m[voice idx][note,ch idx] contains the melody;  Preallocation dummy value C(0)
	std::vector<std::vector<note_t>> m {};
	std::fill_n(std::back_inserter(m),p.nvoice,std::vector<note_t>(p.nnts,cmaj[0]));

	// Rules
	// TODO:  Add namespace qualifiers
	std::vector<bool (*)(const hiller21_status&, const hiller_melody&, const note_t&)> 
		ruleset {
			&line_spans_gt_oct,
			&cf_beginend_tonic,
			&noncf_beginend_tonictriad,
			&no_mel_sevenths,
			&skip_step_rule,
			&rpts_gt_one,
			&rpt_high_nt,
			&harmonic_consonant,
			&harmonic_p4,
			&d_below_tritone,
			&beginend_tonictriad_rootpos,
			&ending_cadence
	};

	// Begin melody generation
	// Each iteration of the loop is responsible for adding a single note new_nt chosen
	// randomly from ntpool.  The target location for new_nt in the melody, managed by the
	// status object s, is m[s.v_idx][s.ch_idx].  Note that s does not contain, and does 
	// not ever mutate m; adding new_nt to m (if it passes all tests) is done manually in
	// the loop.  The status object s effects rejecting notes and rewriting previous portions
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
	// in parameters passed in by the caller (melody_hiller_params p) and are not part of the
	// "status" per se of the process.  Further, this makes the status object useful in other
	// hiller-like functions with different rules, or with situations in which certain rules 
	// apply only on selected iterations or in combination with other rules.  I think that 
	// the "status" object should just keep track of the status, not be responsible for 
	// building m; this is the job of the algorithm writer.  
	// 
	auto re = new_randeng(true);
	std::uniform_int_distribution rd {size_t {0}, ntpool.size()-1};
	hiller21_status s {};
	s.cmaj = &cmaj; s.nnts = p.nnts; s.nvoices = p.nvoice;
	s.rule.resize(ruleset.size(),0);

	std::vector<int> rule_fail_counts(ruleset.size(),0);
	while (s.rejects_tot < p.max_rejects_tot && s.ch_idx < s.nnts) {
		// Draw potential nt to occupy m[v_idx][ch_idx]
		note_t new_nt = ntpool[rd(re)];
		s.new_nt = new_nt;

		// Comparison against the rule set
		for (int i=0; i<ruleset.size(); ++i) {
			bool pass = !((*ruleset[i])(s,m,new_nt));
			s.set_result(i+1,pass);
			if (!pass) {
				break;
			}
		}

		for (int i=0; i<s.rule.size(); ++i) {
			if (s.rule[i] == -1) { ++(rule_fail_counts[i]); }
		}

		if (s.any_failed()) {
			if (p.debug_lvl >= 4) {
				std::cout << s.print(m) << std::endl;
			}

			if (s.rejects_tot > 0 && s.rejects_tot%((p.rejects_regen_ch)*(p.nnts))==0) {
				s.set_for_new_attempt_mel();
			} else if (s.rejects_curr_ch < p.rejects_regen_ch) {
				s.set_for_new_attempt_curr_nt();
			} else {
				s.set_for_new_attempt_prev_ch();
			}
		} else {  // Success:  Go to next note
			if (p.debug_lvl >= 3) {
				std::cout << s.print(m) << std::endl;
			}
			m[s.v_idx][s.ch_idx] = new_nt;
			s.set_for_next();
		}
	}
	
	if (p.debug_lvl >= 2) {
		if (s.rejects_tot >= p.max_rejects_tot) {
			std::cout << "Failed to generate melody; " << std::to_string(s.rejects_tot)
				<< " total rejected iterations exceeded the maximum of "
				<< std::to_string(p.max_rejects_tot) 
				<< ".  \nHere is how far the process got (including junk notes at the end)\n";
		} else {
			std::cout << "\nSuccess!  " << std::to_string(s.rejects_tot) << " total note-rejections\n";
		}
	}

	if (p.debug_lvl >= 1) {
		std::string lpout {};
		for (int ch=0; ch<p.nnts; ++ch) {
			lpout += "<";
			for (int v=0; v<p.nvoice; ++v) {
				lpout += m[v][ch].print(note_t::fmt::lp);
				if (v < (p.nvoice-1)) { lpout += " "; }
			}
			lpout += "> ";
		}
		std::cout << "\n" << lpout << std::endl;
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
	for (auto& e : rule) { e=0; }
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
// total rejects counter, the full resets counter, set ch_idx == 0 and set v_idx == 0
void melody_hiller_internal::hiller21_status::set_for_new_attempt_mel() {
	clear_rules();
	rejects_curr_ch = 0; ++rejects_tot;  ++full_resets_tot;
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
		[](const note_t& lhs,const note_t& rhs){return lhs.frq<rhs.frq;});
}
note_t max_frq (const std::vector<note_t>& nts) {
	return *std::max_element(nts.begin(),nts.end(),
		[](const note_t& lhs,const note_t& rhs){return lhs.frq<rhs.frq;});
}
// Staff line of the given note [1,7]; essentally a "reduced common scale degree."
// Only works if nt corresponds to an scd >= 0
// TODO:  Passing a *; possible to pass a & ???
int reduced_staffln(const diatonic_spn *sc_cmaj, const note_t& nt) {
	auto scd = (*sc_cmaj).to_scd(nt);
	return (scd+1)%8;
}


// Used for std::find(std::vector<note_t>) to find a particular ntlo
bool ntlo_eq(const note_t& lhs, const note_t& rhs) {
	return lhs.ntl == rhs.ntl && lhs.oct == rhs.oct;
}
// Used for std::sort(std::vector<note_t>) to sort by frq
bool ntl_lt_byfrq(const note_t& lhs, const note_t& rhs) {
	return lhs.frq < rhs.frq;
}

// If chord_idx == s.ch_idx, returns notes [0,s.vidx), ie, only the completed notes
// of the working chord.  If chord_idx < s.ch_idx, gets all the notes of that chord:
// [0,s.nvoices).  
std::vector<note_t> get_chord(const hiller21_status& s, const hiller_melody& m, const int chord_idx) {
	int max_v_idx{0};
	if(chord_idx > s.ch_idx) {
		std::abort();
	} else if(chord_idx < s.ch_idx) { // Requesting a ch prior to the working ch
		max_v_idx = s.nvoices;
	} else {  // Requesting the working ch (chord_idx == s.ch_idx)
		max_v_idx = s.v_idx;
	}

	std::vector<note_t> ch {};  ch.reserve(s.nvoices);
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
	std::vector<note_t> voice {};  voice.reserve(s.nnts);
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
// TODO:
// Actually he does:  See p.97.  This routine can be changed to return true for any
// chord containing both a B and an F.  
bool ch_contains_tritone(const std::vector<note_t>& ch) {
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
	auto curr_v = get_voice(s,m,s.v_idx);  curr_v.push_back(new_nt);
	return (max_frq(curr_v).frq/min_frq(curr_v).frq > 2.001);  // TODO:  aprx_gt()
}

// Rule 2
// cf (voice 0) begins+ends on the tonic
bool cf_beginend_tonic(const hiller21_status& s, const hiller_melody& m, const note_t& new_nt) {
	if (!s.first_v() || !(s.first_ch() || s.last_ch())) {
		return false;  // non-cf and/or internal chord
	}
	return new_nt.ntl != ntl_t{"C"};
}

// Rule 3
// Non-cf voices begin+end on tonic triad root position notes
bool noncf_beginend_tonictriad(const hiller21_status& s, const hiller_melody& m, const note_t& new_nt) {
	if (s.first_v() || !(s.first_ch() || s.last_ch())) {
		return false;  // cf and/or internal chord
	}

	if (!(new_nt.ntl == ntl_t {"C"} || new_nt.ntl == ntl_t {"E"}
		|| new_nt.ntl == ntl_t {"G"})) {
		return true;  // fails the rule
	}

	if (s.last_v()) {
		auto curr_ch = get_chord(s,m,s.ch_idx);
		return (min_frq(curr_ch).ntl != ntl_t {"C"});
		// If the lowest note is a C, passes rule (return false)
	}

	return false;
}

// Rule 4
// No melodic intervals == m7 || M7
bool no_mel_sevenths(const hiller21_status& s, const hiller_melody& m, const note_t& new_nt) { 
	if (s.first_ch()) { return false; }
	int staffln_prev_nt = reduced_staffln(s.cmaj, m[s.v_idx][(s.ch_idx-1)]);
	int staffln_new_nt = reduced_staffln(s.cmaj, new_nt);
	return ((staffln_new_nt-staffln_prev_nt) == 7);
}

// Rule 5
// For the current voice, if the prev. two notes are >= an m3 apart (a "skip"), 
// new_nt must be either a repeat of the prev nt or differ from the prev nt by 
// an m2||M2 (a "step").  Their intial description of this rule is confusing; 
// see the better description on p.97-98.  
bool skip_step_rule(const hiller21_status& s, const hiller_melody& m, const note_t& new_nt) {
	if (s.ch_idx < 2) { return false; }
	note_t prev_nt = m[s.v_idx][s.ch_idx-1];
	note_t pprev_nt = m[s.v_idx][s.ch_idx-2];
	int prev_int = std::abs((*s.cmaj).to_scd(prev_nt) - (*s.cmaj).to_scd(pprev_nt));
	if (prev_int >= 2) {
		int new_int = std::abs((*s.cmaj).to_scd(new_nt) - (*s.cmaj).to_scd(prev_nt));
		return !(new_int == 0 || new_int == 1);
	}
	return false;
}

// Rule 6
// No more than one successive repeat of a note is allowed in a given voice.  Require 
// that both the oct and ntl be the same to fail the rule (return true).  
bool rpts_gt_one(const hiller21_status& s, const hiller_melody& m, const note_t& new_nt) {
	if (s.ch_idx < 2) { return false; }
	note_t pprev_nt = m[s.v_idx][s.ch_idx-2];
	note_t prev_nt = m[s.v_idx][s.ch_idx-1];
	return ((pprev_nt.ntl==new_nt.ntl && pprev_nt.oct==new_nt.oct)
		&& (prev_nt.ntl==new_nt.ntl && prev_nt.oct==new_nt.oct));
}

// Rule 7
// Forbidden to repeat the highest note of a line unless the repeat is of the tonic
// and (1) or (2):
// 1)  the repeat resolves a preceeding melodic tritone
// 2)  the repeat involves a cadence to the high tonic: Hence the full sequence is:
//     C(i)-E(i)-G(i)-C(i+1)-C(i+1)
//
// This rule is only defined on a completed line, since only then is it possible to 
// decide if a given repeat is a repeat of the "highest note of the line."  Returns 
// false until the working note is the final note of the line.  
bool rpt_high_nt(const hiller21_status& s, const hiller_melody& m, const note_t& new_nt) {
	if (!s.last_ch()) { return false; }  // line is not complete

	//std::vector<note_t> curr_voice = m[s.v_idx];  curr_voice.push_back(new_nt);
	std::vector<note_t> curr_voice = get_voice(s,m,s.v_idx);  curr_voice.push_back(new_nt);
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
		std::vector<note_t> pre_rpt_mel{curr_voice[i-3],curr_voice[i-2]};
		if (!ch_contains_tritone(pre_rpt_mel)) { return true; }

		// Now check for cadence to high tonic.  
		if (i < 4) { return true; }
		pre_rpt_mel ={curr_voice[i-4],curr_voice[i-3],curr_voice[i-2]};
		bool is_cad_high_tonic = (curr_voice[i-4].ntl == ntl_t{"C"}
			&& curr_voice[i-3].ntl == ntl_t{"E"}
			&& curr_voice[i-2].ntl == ntl_t{"G"}
			&& curr_voice[i-4].frq < curr_voice[i-3].frq
			&& curr_voice[i-3].frq < curr_voice[i-2].frq
			&& curr_voice[i-2].frq < curr_voice[i-1].frq);  // i-1 => first nt of rpt
		if (!is_cad_high_tonic) { return true; }
	}  // to next nt i of curr_voice
	
	return false;
}

// Rule 8
// For each chord, only "consonant" harmonic intervals are permitted (includes, but
// not limited to: U, m3, M3, P5, 6'ths, O).  
// Forbid:  m2, M2, m7, M7
// The tritone is dissonant, but is allowed in some cases; see rule 10 for enforcement
// of this.  
// He is considering _reduced_ intervals.  C(3)->D(5) is an M2.  See p.103-105.  
bool harmonic_consonant(const hiller21_status& s, const hiller_melody& m, const note_t& new_nt) {
	if (s.first_v()) { return false; }

	int rstaffln_newnt = reduced_staffln(s.cmaj, new_nt);
	std::vector<note_t> curr_ch = get_chord(s,m,s.ch_idx);
	for (const auto& e : curr_ch) {
		int rstaffdist = rstaffln_newnt-reduced_staffln(s.cmaj, e);
		if (rstaffdist == 7 || rstaffdist == 2) {
			return true;
		}
	}

	return false;
}

// Rule 9
// For each chord, a P4 is allowed iff it does not occur between the lowest 
// note of the chord and an upper voice.  Returns false until s.last_voice(); only 
// then is it possible to define the "lowest note of the chord."
bool harmonic_p4(const hiller21_status& s, const hiller_melody& m, const note_t& new_nt) {
	if (!s.last_v()) { return false; }

	std::vector<note_t> curr_ch = get_chord(s,m,s.ch_idx);  curr_ch.push_back(new_nt);
	note_t lowest_nt = min_frq(curr_ch);
	
	if (lowest_nt.ntl == ntl_t {"F"}) {
		// In the Cmaj scale, the note 3 staff positions above F is B, but F->B is 6
		// semitones and thus is not a "perfect" 4'th.  For all other possible values
		// of lowest_nt, the note 3 staff positions higher is 5 semitones away, thus 
		// the interval is a perfect fourth.  
		return false;
	}

	for (const auto& e : curr_ch) {
		int staff_interval = (*s.cmaj).to_scd(e)-(*s.cmaj).to_scd(lowest_nt);
		if (staff_interval==3) {
			return true;  // NB: A dist of 3 staff positions is a "fourth"
		}
	}
	return false;
}

// Rule 10
// Part a:  True if a tritone (B(i)->F(i+1) in Cmaj) occurs in the present chord w/o 
// a D below the B; false otherwise.  
// Part b:  This tritone must resolve to E-C or C-E in the next chord.  
// TODO:  Part b not implemented.  
// Always returns false if s.v_idx indicates that the present chord is incomplete, since
// only then is it possible to know if a D occurs below the B involved in the tritone.  
bool d_below_tritone(const hiller21_status& s, const hiller_melody& m, const note_t& new_nt) {
	if (!s.last_v()) { return false; }

	auto curr_ch = get_chord(s,m,s.ch_idx);  curr_ch.push_back(new_nt);
	if (!ch_contains_tritone(curr_ch)) { return false; }

	// Build test_ch, containing all notes from curr_ch except B notes above 
	// the lowest D note.  
	std::sort(curr_ch.begin(),curr_ch.end(),ntl_lt_byfrq);
	bool found_d {false};  std::vector<note_t> test_ch{};
	for (const auto& e : curr_ch) {
		if (e.ntl == ntl_t {"D"}) { found_d = true; }
		if (found_d && e.ntl == ntl_t {"B"}) {
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
}

// Rule 11
// The lowest note in the first and last chords must be the tonic
bool beginend_tonictriad_rootpos(const hiller21_status& s, const hiller_melody& m,
	const note_t& new_nt)  {
	if (!s.last_v() || !(s.first_ch() || s.last_ch())) {
		return false;  // curr pos != last-voice and/or curr pos == an internal chord
	}
	auto curr_ch = get_chord(s,m,s.ch_idx);  curr_ch.push_back(new_nt);
	return (min_frq(curr_ch).ntl != ntl_t {"C"});
}

// Rule 12
// The second-last chord must contain the note B in exactly one voice.  In the last chord,
// this voice must be the note C.  
// Always returns false if the working note is not the final note of the second-to-last
// chord.  
bool ending_cadence(const hiller21_status& s, const hiller_melody& m, const note_t& new_nt) {
	return ch_nxtlast_contains_b(s,m,new_nt) || lastch_contains_rsln_from_b(s,m,new_nt);
};
bool ch_nxtlast_contains_b(const hiller21_status& s, const hiller_melody& m, const note_t& new_nt) {
	if (!(s.last_v() && s.ch_idx == (s.nnts-2))) {  // !s.last_v() || s.ch_idx != (s.nnts-2)
		return false;
	}
	std::vector<note_t> ch = get_chord(s,m,s.ch_idx);  ch.push_back(new_nt);
	int num_b {0};
	for(const auto& e : ch) {
		if (e.ntl == ntl_t {"B"}) { ++num_b; }
	}
	return num_b != 1;
};
bool lastch_contains_rsln_from_b(const hiller21_status& s, const hiller_melody& m,
	const note_t& new_nt) {
	if (!(s.last_v() && s.last_ch())) { return false; }  // (!s.last_v() || !s.last_ch())
	std::vector<note_t> ch_nxtlast = get_chord(s,m,s.ch_idx-1);
	std::vector<note_t> ch_last = get_chord(s,m,s.ch_idx); ch_last.push_back(new_nt);
	for(int i=0; i<ch_nxtlast.size(); ++i) {  // ch_nxtlast.size() == ch_last.size()
		if (ch_nxtlast[i].ntl == ntl_t {"B"}) {
			return ch_last[i].ntl != ntl_t {"C"};
			// == C => rule passes => return false
		}
	}
	// Means ch_nxtlast did not contain a B; rule 12a should prevent this
	return true;
};

}  // namespace melody_hiller_internal




