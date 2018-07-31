#include <string>
#include <vector>
#include <optional>
#include <cmath>
#include "scale_12tet.h"
#include "types\ntl_t.h"
#include "types\frq_t.h"
#include "types\scd_t.h"
#include "util\au_util_all.h"

scale_12tet::scale_12tet() {
	m_name = "12-tone equal-tempered scale";
	m_name += " with " + ntstr_t{m_ref_ntl,m_ref_octn}.print() + " defined as ";
	m_name += m_ref_frq.print() + " Hz.";
	m_description = "Default-constructed";
}

scale_12tet::scale_12tet(ntl_t ref_ntl_in, octn_t ref_oct_in, frq_t ref_frq_in) {
	auto boolidx = ismember(m_default_valid_ntls,ref_ntl_in);
	if (!isany(boolidx)) {
		au_error("Invalid ref ntl");
	}
	auto ref_ntl_idx = bool2idx(boolidx);

	int m_ref_ntl_idx {ref_ntl_idx[0]};
	ntl_t m_ref_ntl {ref_ntl_in};
	frq_t m_ref_frq {ref_frq_in};
	octn_t m_ref_octn {ref_oct_in};

	m_name = "12-tone equal-tempered scale";
	m_name += " with " + ntstr_t{m_ref_ntl,m_ref_octn}.print() + " defined as ";
	m_name += m_ref_frq.print() + " Hz.";

	m_description = "scale_12tet::scale_12tet(ntl_t " + ref_ntl_in.print();
	m_description += ", octn_t " + ref_oct_in.print();
	m_description += ", frq_t " + ref_frq_in.print() + ")";
}

int scale_12tet::n() {
	return m_n;
}

std::string scale_12tet::name() const {
	return m_name;
}
std::string scale_12tet::description() const {
	return m_description;
}

// Static data members
const int scale_12tet::m_gint = 2;  // Generating interval = 2
const int scale_12tet::m_n = 12;  // ntet = 12
const std::vector<ntl_t> scale_12tet::m_default_valid_ntls
	{"C"_ntl,"C#"_ntl,"D"_ntl,"D#"_ntl,"E"_ntl,"F"_ntl,"F#"_ntl,"G"_ntl,
	"G#"_ntl,"A"_ntl,"A#"_ntl,"B"_ntl};
const int scale_12tet::m_default_ref_ntl_idx = 9; // "A"
const frq_t scale_12tet::m_default_ref_frq {440};
const octn_t scale_12tet::m_default_ref_octave {4};

// This function assumes none of the ntls in the scale are duplicates, so it
// is not applicable to the completely general case of an arbitrary scale
ntstr_t scale_12tet::to_ntstr(scd_t scd_in) {  // Reads m_default_valid_ntls directly
	rscdoctn_t rscdoctn_in {scd_in, m_n};
	return ntstr_t {m_default_valid_ntls[rscdoctn_in.to_int()], 
		octn_t{scd_in,m_n}};
}

std::optional<ntstr_t> scale_12tet::to_ntstr(frq_t frq_in) {  // wrapper
	std::optional<scd_t> scd_in = to_scd(frq_in);
	if (!scd_in) {
		return {};
	}
	return to_ntstr(*scd_in);
}

frq_t scale_12tet::to_frq(scd_t scd_in) {  // Calls frq_eqt() directly
	auto dn = scd_in.to_int()-(m_ref_ntl_idx + m_n*m_ref_octn.to_int());
	frq_t frq = frq_eqt(dn, m_ref_frq, m_n, m_gint);
	return frq;
}
std::optional<frq_t> scale_12tet::to_frq(ntstr_t ntstr_in) {  // wrapper
	std::optional<scd_t> scd_in = to_scd(ntstr_in);
	if (!scd_in) {
		return {};
	}

	return to_frq(*scd_in);
}
std::optional<scd_t> scale_12tet::to_scd(frq_t frq_in) {  //  Calls n_eqt() directly
	auto dn_approx = n_eqt(frq_in, m_ref_frq, m_n, m_gint);
	if (!isapproxint(dn_approx,6)) {
		return {};
	}
	auto dn = static_cast<int>(std::round(dn_approx));
	
	return scd_t {dn - m_ref_octn.to_int()*m_n + m_ref_ntl_idx};
}

// This function assumes none of the ntls in the scale are duplicates, so it
// is not applicable to the completely general case of an arbitrary scale
std::optional<scd_t> scale_12tet::to_scd(ntstr_t ntstr_in) {  // Reads m_default_valid_ntls directly
	auto boolidx = ismember(m_default_valid_ntls,ntl_t{ntstr_in});
	if (!isany(boolidx)) {
		return {};
	}
	auto ntl_idx = bool2idx(boolidx);
	auto rscdoct = rscdoctn_t{scd_t{ntl_idx[0]},m_n};
	return rscdoct.to_scd(octn_t{ntstr_in}); // barf the conversion operator
	//return scd_t{ntl_idx[0]};
}

octn_t scale_12tet::to_octn(scd_t scd_in) {
	//rscdoctn_t rscdoctn_in {scd_in,m_n};
	//return octn_t{rscdoctn_in.to_octn().to_int()};
	return octn_t{scd_in,m_n};
}

std::optional<octn_t> scale_12tet::to_octn(frq_t frq_in) {
	std::optional<scd_t> scd_in = to_scd(frq_in);
	if (!scd_in) {
		return {};
	}
	return to_octn(*scd_in);
}

std::optional<octn_t> scale_12tet::to_octn(ntstr_t ntstr_in) {
	std::optional<scd_t> scd_in = to_scd(ntstr_in);
	if (!scd_in) {
		return {};
	}
	return to_octn(*scd_in);
}

bool scale_12tet::isinsc(frq_t frq_in) {
	double dn_approx = n_eqt(frq_in, m_ref_frq, m_n, m_gint);
	return (isapproxint(dn_approx,6)); 
}
bool scale_12tet::isinsc(ntl_t ntl_in) {
	return isany(ismember(m_default_valid_ntls,ntl_in));
}









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

std::vector<frq_t> frq_eqt(std::vector<int> const& dn, frq_t const& ref_frq, \
	int const& ntet, int const& gint) {
	std::vector<frq_t> frqs; frqs.reserve(dn.size());

	// f = fref.*((gint^(1/ntet))).^dn;
	auto x = std::pow(static_cast<double>(gint),(1/static_cast<double>(ntet)));
	for (auto cn : dn) {
		auto cfrq = frq_eqt(cn, ref_frq, ntet, gint);
		frqs.push_back(cfrq);
	}
	return frqs;
}

frq_t frq_eqt(int const& dn, frq_t const& ref_frq, \
	int const& ntet, int const& gint) {
	// f = fref.*((gint^(1/ntet))).^dn;
	auto x = std::pow(static_cast<double>(gint),(1/static_cast<double>(ntet)));
	frq_t frq = ref_frq * std::pow(x, static_cast<double>(dn));
	
	return frq;
}

// The inverse:  Takes in a frq_t and returns a double corresponding to the scd_t
double n_eqt(frq_t const& frq_in, frq_t const& ref_frq, \
	int const& ntet, int const& gint) {
	// f = fref.*((gint^(1/ntet))).^dn;
	// => dn = log(f/fref)/log(gint^(1/ntet))
	double gint_pow_ntet = std::pow(static_cast<double>(gint),(1/static_cast<double>(ntet)));
	double ffref = frq_in/ref_frq;
	double scd_approx = std::log(ffref)/std::log(gint_pow_ntet);


	return scd_approx;
}



