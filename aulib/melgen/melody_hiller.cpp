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


std::vector<note_t> melody_hiller_ex11(const melody_hiller_params& p) {
	// 1)  No tritones
	// 2)  No sevenths
	// 3)  Start, end on middle C
	// 4)  max(melody)-min(melody) <= 1 octave

	bool passed {false};
	while (!passed) {
		//...
	}

}



//
// It should also be possible to implement this in the same way as melody_a(), ie, by first
// generating a completely random 2-d array of notes, scoring according to the counterpoint
// rules, mutating, re-scoring, etc...
//
//
//
std::vector<note_t> melody_hiller_ex21(const melody_hiller_params& p) {
	diatonic_spn sc {ntl_t {"C"}, diatonic_spn::mode::major};
	int nnts {12};
	int nvoices {2};
	int scd_min = sc.to_scd(ntl_t{"C"},octn_t{3});
	int scd_max = sc.to_scd(ntl_t{"C"},octn_t{5});
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

	auto ntclass_eq = [](const note_t& lhs, const note_t& rhs) -> bool {
		return lhs.ntl == rhs.ntl;
	};

	auto ntl_ismember = [ntclass_eq](const std::vector<note_t>& ch, const ntl_t& nt) -> bool {
		for (int i=0; i<ch.size(); ++i) {
			if (ch[i].ntl==nt) {
				return true;
			}
		}
		return false;
	};

	// m[voice][note]
	auto get_chord = [&sc, &m, &nvoices](int chord_idx) -> std::vector<note_t> {
		std::vector<note_t> result {}; result.reserve(nvoices);
		for (int i=0; i<nvoices; ++i) {  // m.size() == nvoices
			result.push_back(m[i][chord_idx]);
		}
		return result;
	};
	// m[voice][note]
	auto get_voice = [&sc, &m, &nnts](int voice_idx) -> std::vector<note_t> {
		std::vector<note_t> result {}; result.reserve(nnts);
		for (int i=0; i<nnts; ++i) {  // m.size() == nvoices
			result.push_back(m[voice_idx][i]);
		}
		return result;
	};

	// True if any note in the second-to-last and last chords of m contain a B and F, or an F and B,
	// respectively.  That is, if there is a tritone betweeen any two voices in the last two chords.
	// False otherwise
	auto tritone = [&sc, &m, get_chord, ntl_ismember](int last_ch_idx) -> bool {
		if (last_ch_idx >= 1) {
			auto ch_last = get_chord(last_ch_idx);
			auto ch_nxtlast = get_chord(last_ch_idx);
			return (ntl_ismember(ch_last,ntl_t{"F"}) &&	ntl_ismember(ch_nxtlast,ntl_t{"B"}) ||
				ntl_ismember(ch_last,ntl_t{"B"}) &&	ntl_ismember(ch_nxtlast,ntl_t{"F"}));
		}
		return false;
	};

	// True if the input notes correspond to a major or minor seventh.  False otherwise.  
	auto seventh = [&sc](const note_t& a, const note_t& b) -> bool {
		return (std::abs(sc.to_scd(a)-sc.to_scd(b)) == 7);
	};
	// True if the input notes correspond to a step >= m3.  False otherwise.  
	auto geq_third = [&sc](const note_t& a, const note_t& b) -> bool {
		return (std::abs(sc.to_scd(a)-sc.to_scd(b)) >= 3);
	};
	// True if the input notes correspond to a step == m3 or M3.  False otherwise.  
	auto eq_third = [&sc](const note_t& a, const note_t& b) -> bool {
		return (std::abs(sc.to_scd(a)-sc.to_scd(b)) == 3);
	};
	// True if the input notes correspond to a step >= m3.  False otherwise.  
	auto eq_second = [&sc](const note_t& a, const note_t& b) -> bool {
		return (std::abs(sc.to_scd(a)-sc.to_scd(b)) == 2);
	};

	// Rule 1
	auto span_geq_oct = [min_frq](const std::vector<note_t>& v, const note_t& new_nt) -> bool { 
		return (new_nt.frq/min_frq(v).frq >= 2.0 || new_nt.frq/min_frq(v).frq <= 0.5);
	};

	// Rule 2 - Implemented implictly
	// Rule 3 - Implemented implictly
	// Rule 4 - Direct call to seventh()

	// Rule 5
	// If the prev. two notes are >= an m3 apart (a "skip"), the new_nt must be either a repeat 
	// of the prev nt or differ from the prev nt by an m2 (a "step").  If the prev two notes
	// are a step, new_nt must be either a step or a skip.  
	auto skip_step_rule = [geq_third, eq_second, eq_third](const std::vector<note_t>& v, const note_t& new_nt) -> bool { 
		if (v.size() == 1) {
			return false;
		}
		auto n = v.size();
		if (geq_third(v[n-2],v[n-1])) {  // Prev 2 notes are a skip
			return !(new_nt.ntl == v[n-1].ntl || eq_second(v[n-1],new_nt));
		} else if (eq_second(v[n-2],v[n-1])) {  // Prev 2 notes are a step
			return !(eq_second(v[n-1],new_nt) || eq_third(v[n-1],new_nt));
		}
		return false;
	};

	// Rule 6
	auto rpts_gt_one = [](const std::vector<note_t>& v, const note_t& new_nt) -> bool { 
		if (v.size() == 1) {
			return false;
		} else {
			auto n = v.size();
			return (v[n-1].ntl == new_nt.ntl && v[n-2].ntl == new_nt.ntl);
		}
	};

	// Rule 7
	// Forbidden to repeat the highest note of a line unless said note is the tonic AND
	// the line moves in a tritone resolution or a cadence to the high tonic.  
	// At present only checks for repeats of the highest note on a given line and that
	// said repeat is of C.  
	// Rule is only defined on a completed line, since only then can i define the "highest
	// note."  Since the melody is generated one chord at a time, the lines are all completed
	// together with the full melody.  
	auto rpt_high_nt = [max_frq](const std::vector<std::vector<note_t>>& m) -> bool { 
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


	std::vector<note_t> curr_ntpool {};
	for (int i=0; i<nnts; ++i) {  // m[voice][note]
		for (int j=0; j<nvoices; ++j) {  // m[voice][note]
			//
			// Set curr_ntpool
			//
			curr_ntpool = ntpool;
			if (i==0) {  // first note of the melody
				if (j==0) {  // cf
					// Special section needed only at the beginning of the cf.  Set the tonic chord
					// in root position to the first position in the melody.  
					//scd_t scd = sc.to_scd(ntl_t{"C"},octn_t{4});
					//m[i][j] = sc[scd];
					curr_ntpool = ntpool_tonic_nt;
				} else if (j > 0) {  // not the cf
					curr_ntpool = ntpool_tonic_ch;
				}
			} else { // not the first note
				//...
			}

			//
			// Draw potential note j
			//
			note_t new_nt = curr_ntpool[rd(re)%curr_ntpool.size()];

			//
			// Compare new_nt against the rule set
			//
			if (seventh(m[i][j-1],new_nt) || span_geq_oct(m[i],new_nt) || rpts_gt_one(m[i],new_nt)
				|| skip_step_rule(m[i],new_nt)) {
				// reject new_nt
			}
			
		}  // to next voice of present chord

	}  // to next chord of first voice


	if (rpt_high_nt(m)) {
		// discard melody
	}

}
