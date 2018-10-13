#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include "scale_12tet.h"
#include "..\types\cent_oct_t.h"
#include "..\types\ntl_t.h"
#include "..\types\frq_t.h"
#include "..\types\scd_t.h"
#include "..\util\au_util.h"
#include "..\util\au_error.h"
#include "..\util\au_algs.h"  // ismember()
#include "..\util\au_algs_math.h"  //aprx_int()


// Static data members
const int scale_12tet::m_gint = 2;  // Generating interval = 2
const int scale_12tet::m_n = 12;  // ntet = 12
const std::vector<ntl_t> scale_12tet::m_default_valid_ntls
	{"C"_ntl,"C#"_ntl,"D"_ntl,"D#"_ntl,"E"_ntl,"F"_ntl,"F#"_ntl,"G"_ntl,
	"G#"_ntl,"A"_ntl,"A#"_ntl,"B"_ntl};
const int scale_12tet::m_default_ref_ntl_idx = 9; // "A"
const frq_t scale_12tet::m_default_ref_frq {440};
const octn_t scale_12tet::m_default_ref_octave {4};

scale_12tet::scale_12tet() {
	m_name = bsprintf("12-tone equal-tempered scale with %s defined as %s Hz",
		ntstr_t{m_ref_ntl,m_ref_octn}.print(), m_ref_frq.print());
	m_description = "Default-constructed";
}

// Means that ref_ntl_in(ref_oct_in) corresponds to ref_frq_in.  Note however that 
// scd_t {0} does _not_ correspond to ref_ntl_in(ref_oct_in).  scd_t {0} always
// corresponds to ref_ntl_in(0).  
//
// to_ntstr(my_scd) == to_scd(to_ntstr(my_scd))
// to_frq(my_scd) == to_scd(to_frq(my_scd))
//
scale_12tet::scale_12tet(ntl_t ref_ntl_in, octn_t ref_oct_in, frq_t ref_frq_in) {
	auto it = std::find(m_default_valid_ntls.begin(),
		m_default_valid_ntls.end(),ref_ntl_in);
	au_assert(ismember(ref_ntl_in,m_default_valid_ntls),"Invalid ref ntl");

	int m_ref_ntl_idx =static_cast<int>(it-m_default_valid_ntls.begin());
	m_ref_ntl = ref_ntl_in;
	m_ref_frq = ref_frq_in;
	m_ref_octn = ref_oct_in;

	m_name = bsprintf("12-tone equal-tempered scale with %s defined as %s Hz",
		ntstr_t{m_ref_ntl,m_ref_octn}.print(), m_ref_frq.print());

	m_description = bsprintf("scale_12tet(ntl_t %s, octn_t %s, frq_t %s)",
		ref_ntl_in.print(), ref_oct_in.print(), ref_frq_in.print());
}

int scale_12tet::n() const {
	return m_n;
}
std::string scale_12tet::name() const {
	return m_name;
}
std::string scale_12tet::description() const {
	return m_description;
}
bool scale_12tet::isinsc(ntl_t ntl_in) const {
	return ismember(ntl_in, m_default_valid_ntls);
}
bool scale_12tet::isinsc(frq_t frq_in) const {
	return aprx_int(n_eqt(frq_in, m_ref_frq, m_n, m_gint));
}


// This function assumes none of the ntls in the scale are duplicates, so it
// is not applicable to the completely general case of an arbitrary scale
ntstr_t scale_12tet::to_ntstr(scd_t scd_in) {  // Reads m_default_valid_ntls directly
	rscdoctn_t rscdoctn_in {scd_in, m_n};
	//return ntstr_t {m_default_valid_ntls[rscdoctn_in.to_int()], 
	//	octn_t{scd_in,m_n}};
	return ntstr_t {m_default_valid_ntls[(rscdoctn_in.to_int()+m_ref_ntl_idx)%12], 
		octn_t{scd_in,m_n}};
}
std::vector<ntstr_t> scale_12tet::to_ntstr(std::vector<scd_t> scds_in) {
	std::vector<ntstr_t> ntstrs(scds_in.size(),ntstr_t{});
	for (int i=0; i<scds_in.size(); ++i) {
		ntstrs[i] = to_ntstr(scds_in[i]);
	}
	return ntstrs;
}

ntstr_t scale_12tet::to_ntstr(frq_t frq_in) {  // wrapper
	scd_t scd_in = to_scd(frq_in);
	return to_ntstr(scd_in);
}

frq_t scale_12tet::to_frq(scd_t scd_in) {  // Calls frq_eqt() directly
	auto dn = scd_in.to_int()-(m_ref_ntl_idx + m_n*m_ref_octn.to_int());
	frq_t frq = frq_eqt(dn, m_ref_frq, m_n, m_gint);
	return frq;
}
frq_t scale_12tet::to_frq(ntstr_t ntstr_in) {  // wrapper
	scd_t scd_in = to_scd(ntstr_in);
	return to_frq(scd_in);
}
scd_t scale_12tet::to_scd(frq_t frq_in) {  //  Calls n_eqt() directly
	double dn_approx = n_eqt(frq_in, m_ref_frq, m_n, m_gint);
	au_assert(aprx_int(dn_approx),"frq not in sc");
	auto dn = static_cast<int>(std::round(dn_approx));
	
	//return scd_t {dn - m_ref_octn.to_int()*m_n + m_ref_ntl_idx};
	scd_t ans_scd {dn + (m_ref_octn.to_int()*m_n + m_ref_ntl_idx)};
	return ans_scd;
}

// This function assumes none of the ntls in the scale are duplicates, so it
// is not applicable to the completely general case of an arbitrary scale
scd_t scale_12tet::to_scd(ntstr_t ntstr_in) {  // Reads m_default_valid_ntls directly
	auto it = std::find(m_default_valid_ntls.begin(),
		m_default_valid_ntls.end(),ntl_t{ntstr_in});
	au_assert(it != m_default_valid_ntls.end(), "ntl not in sc");
	int ntl_idx = it-m_default_valid_ntls.begin();
	//auto rscdoct = rscdoctn_t{scd_t{ntl_idx},m_n};
	auto rscdoct = rscdoctn_t{scd_t{ntl_idx-m_ref_ntl_idx},m_n};
	return rscdoct.to_scd(octn_t{ntstr_in}); // barf: using the conversion operator
}

octn_t scale_12tet::to_octn(scd_t scd_in) {
	return octn_t{scd_in,m_n};
}

octn_t scale_12tet::to_octn(frq_t frq_in) {
	scd_t scd_in = to_scd(frq_in);
	return to_octn(scd_in);
}

octn_t scale_12tet::to_octn(ntstr_t ntstr_in) {
	scd_t scd_in = to_scd(ntstr_in);
	return to_octn(scd_in);
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

std::vector<frq_t> frq_eqt(std::vector<int> const& dn, frq_t const& ref_frq, 
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

frq_t frq_eqt(int const& dn, frq_t const& ref_frq, 
	int const& ntet, int const& gint) {
	// f = fref.*((gint^(1/ntet))).^dn;
	auto x = std::pow(static_cast<double>(gint),(1/static_cast<double>(ntet)));
	frq_t frq = ref_frq * std::pow(x, static_cast<double>(dn));
	
	return frq;
}

// The inverse:  Takes in a frq_t and returns a double corresponding to the scd_t
double n_eqt(frq_t const& frq_in, frq_t const& ref_frq, 
	int const& ntet, int const& gint) {
	// f = fref.*((gint^(1/ntet))).^dn;
	// => dn = log(f/fref)/log(gint^(1/ntet))
	double gint_pow_ntet = std::pow(static_cast<double>(gint),(1/static_cast<double>(ntet)));
	double ffref = frq_in/ref_frq;
	double scd_approx = std::log(ffref)/std::log(gint_pow_ntet);

	return scd_approx;
}

