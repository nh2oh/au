#include "scale.h"



//
// Return a frq vector corresponding to some equal-tempered scale.  
// Calculate the frequency of notes dn semitones different from the reference
// note, defined by fref in an ntet-tone equal temperment syetem in which a 
// "generating interval" gint is divided into ntet equal divisions.  For the
// standard "12-TET" A440 system used by essentially everybody, fref=440, 
// ntet=12, gint=2 (the generating interval for 12-TET is the octave, thus 
// gint=2).  
//
// For the equal-tempered Bohlen–Pierce scale, which divides the interval 3
// (as opposed to the octave, 2) into 13 divisions (but still sticking with 
// the convention that "A4" is 440 Hz):
// f = frq_eqt(-9,440,13,3) gives the note C# (or D-flat), and:
// f = frq_eqt(-1,440,13,3) gives the note J
//
// For example, to calculate C4 ("middle C") in 12-TET using A4-440 as the 
// reference tone:
// f = frq_eqt(-9,440,12,2), because C4 is 9 semitones below A4
//
// To generate the standard 12-tone chromatic scale starting on C(0):
// std::vector<int> dn_vec {-57,-56,-55,-54,-53,-52,-51,-50,-49,-48,-47,-46};
// auto cchrom_new_frqs = frq_eqt(dn_vec, 440, 12, 2);
//

std::vector<frq_t> frq_eqt(const std::vector<int>& dn, frq_t ref_frq, 
	int ntet, int gint) {
	std::vector<frq_t> frqs; frqs.reserve(dn.size());

	for (const auto& cn : dn) {
		auto cfrq = frq_eqt(cn, ref_frq, ntet, gint);
		frqs.push_back(cfrq);
	}
	return frqs;
}

frq_t frq_eqt(int dn, frq_t ref_frq, int ntet, int gint) {
	// f = fref.*((gint^(1/ntet))).^dn;
	frq_t frq {ref_frq*std::pow(gint,dn/static_cast<double>(ntet))};
	return frq;
}

// The inverse:  Takes in a frq_t and returns a double corresponding to the scd_t
double n_eqt(frq_t frq_in, frq_t ref_frq, int ntet, int gint) {
	return ntet*(std::log(frq_in/ref_frq)/std::log(static_cast<double>(gint)));
}


