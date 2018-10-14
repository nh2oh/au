#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include "spn12tet.h"
#include "..\types\cent_oct_t.h"
#include "..\types\ntl_t.h"
#include "..\types\frq_t.h"
#include "..\types\scd_t.h"
#include "..\util\au_util.h"
#include "..\util\au_error.h"
#include "..\util\au_algs.h"  // ismember()
#include "..\util\au_algs_math.h"  //aprx_int()

/*
// Static data members
const int scale_12tet::m_gint = 2;  // Generating interval = 2
const int scale_12tet::m_n = 12;  // ntet = 12
const std::vector<ntl_t> scale_12tet::m_default_valid_ntls
	{"C"_ntl,"C#"_ntl,"D"_ntl,"D#"_ntl,"E"_ntl,"F"_ntl,"F#"_ntl,"G"_ntl,
	"G#"_ntl,"A"_ntl,"A#"_ntl,"B"_ntl};
const int scale_12tet::m_default_ref_ntl_idx = 9; // "A"
const frq_t scale_12tet::m_default_ref_frq {440};
const octn_t scale_12tet::m_default_ref_octave {4};
*/


// Means that ref_ntl_in(ref_oct_in) corresponds to ref_frq_in.  Note however that 
// scd_t {0} does _not_ correspond to ref_ntl_in(ref_oct_in).  scd_t {0} always
// corresponds to ref_ntl_in(0).  
//
// to_ntstr(my_scd) == to_scd(to_ntstr(my_scd))
// to_frq(my_scd) == to_scd(to_frq(my_scd))
//

spn12tet::spn12tet(spn12tet::pitch_std ps) {
	au_assert(ps.gen_int > 0,"ono");
	au_assert(ps.ntet>0,"ono");
	m_ps = ps;
	auto idx = std::find(m_ntls.begin(),m_ntls.end(),ntl_t{m_ps.ref_ntstr})-m_ntls.begin();
	m_shift_scd = (octn_t{ps.ref_ntstr}.to_int())*12+static_cast<int>(idx);
	// TODO:  Expect 57 ??
}

int spn12tet::n() const {
	return 12;
}
std::string spn12tet::name() const {
	return m_name;
}
std::string spn12tet::description() const {
	return m_description;
}
bool spn12tet::isinsc(ntl_t ntl_in) const {
	return ismember(ntl_in, m_ntls);
}
bool spn12tet::isinsc(frq_t frq_in) const {
	return aprx_int(n_eqt(frq_in, m_ps.ref_frq, m_ps.ntet, m_ps.gen_int));
}
std::string spn12tet::print(scd_t from, scd_t to) const {
	std::string s {};
	s = bsprintf("%s\n%s\n\n",m_name,m_description);

	for (scd_t curr_scd=from; curr_scd<to; ++curr_scd) {
		//std::string curr_line {};
		s += bsprintf("%d:\t%s\t%.3f\n",
			curr_scd.to_int(),to_ntstr(curr_scd).print(),to_frq(curr_scd)/frq_t{1});
	}
	s += "\n\n";
	return s;
}

ntstr_t spn12tet::to_ntstr(scd_t scd) const {  // Reads m_default_valid_ntls directly
	rscdoctn_t rscd {scd,12};
	return ntstr_t {m_ntls[rscd.to_int()], octn_t{scd,12}};
}
std::vector<ntstr_t> spn12tet::to_ntstr(const std::vector<scd_t>& scds) const {
	std::vector<ntstr_t> ntstrs(scds.size(),ntstr_t{});
	for (int i=0; i<scds.size(); ++i) {
		ntstrs[i] = to_ntstr(scds[i]);
	}
	return ntstrs;
}
ntstr_t spn12tet::to_ntstr(frq_t frq) const {  // wrapper
	scd_t scd = to_scd(frq);
	return to_ntstr(scd);
}

frq_t spn12tet::to_frq(scd_t scd) const {  // Calls frq_eqt() directly
	return spn12tet::frq_eqt(scd.to_int(),m_ps.ref_frq,m_ps.ntet,m_ps.gen_int);
}
std::vector<frq_t> spn12tet::to_frq(const std::vector<scd_t>& scds) const {
	std::vector<frq_t> res(scds.size(),frq_t{});
	for (int i=0; i<scds.size(); ++i) {
		res[i] = to_frq(scds[i]);
	}
	return res;
}
frq_t spn12tet::to_frq(ntstr_t ntstr) const {  // wrapper
	scd_t scd = to_scd(ntstr);
	return to_frq(scd);
}
std::vector<frq_t> spn12tet::to_frq(const std::vector<ntstr_t>& ntstr) const {
	std::vector<frq_t> res(ntstr.size(),frq_t{});
	for (int i=0; i<ntstr.size(); ++i) {
		res[i] = to_frq(ntstr[i]);
	}
	return res;
}
scd_t spn12tet::to_scd(frq_t frq) const {  //  Calls n_eqt() directly
	double dn_approx = n_eqt(frq,m_ps.ref_frq,m_ps.ntet,m_ps.gen_int);
	au_assert(aprx_int(dn_approx),"frq not in sc");
	return scd_t {static_cast<int>(std::round(dn_approx))};
}
scd_t spn12tet::to_scd(ntstr_t ntstr) const {  // Reads m_default_valid_ntls directly
	auto it = std::find(m_ntls.begin(),m_ntls.end(),ntl_t{ntstr});
	au_assert(it != m_ntls.end(), "ntl not in sc");
	int ntl_idx = it-m_ntls.begin();
	int octn = octn_t{ntstr}.to_int();
	return scd_t {ntl_idx+12*octn};
}
std::vector<scd_t> spn12tet::to_scd(const std::vector<ntstr_t>& ntstr) const {
	std::vector<scd_t> res(ntstr.size(),scd_t{});
	for (int i=0; i<ntstr.size(); ++i) {
		res[i] = to_scd(ntstr[i]);
	}
	return res;
}

octn_t spn12tet::to_octn(scd_t scd) const {
	return octn_t{scd,12};
}
octn_t spn12tet::to_octn(frq_t frq) const {
	scd_t scd = to_scd(frq);
	return to_octn(scd);
}


// TODO:  Check these examples
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

std::vector<frq_t> spn12tet::frq_eqt(const std::vector<int>& dn, frq_t ref_frq, 
	int ntet, int gint) const {
	std::vector<frq_t> frqs; frqs.reserve(dn.size());

	for (const auto& cn : dn) {
		auto cfrq = frq_eqt(cn, ref_frq, ntet, gint);
		frqs.push_back(cfrq);
	}
	return frqs;
}

frq_t spn12tet::frq_eqt(int dn, frq_t ref_frq, int ntet, int gint) const {
	// f = fref.*((gint^(1/ntet))).^dn;
	double dn_shift = dn - m_shift_scd;
	//auto x = std::pow(static_cast<double>(gint),(1/static_cast<double>(ntet)));
	//frq_t frq = ref_frq * std::pow(x, static_cast<double>(dn));
	frq_t frq {ref_frq*std::pow(gint,dn_shift/static_cast<double>(ntet))};
	return frq;
}

// The inverse:  Takes in a frq_t and returns a double corresponding to the scd_t
double spn12tet::n_eqt(frq_t frq_in, frq_t ref_frq, int ntet, int gint) const {
	// f = fref*(gint^(1/ntet))^dn;
	// =>  fref*(gint^(dn/ntet));
	// => log(f/fref) = (dn/ntet)*log(gint)
	// => dn/ntet = log(f/fref)/log(gint)
	// => dn = ntet*(log(f/fref)/log(gint))
	
	
	double dn = ntet*(std::log(frq_in/ref_frq)/std::log(static_cast<double>(gint)));
	if (aprx_int(dn)) {
		dn = std::round(dn);
	}
	return dn + m_shift_scd;
	//bool tf1 = aprx_int(x);
	//double res = x+m_shift_scd;
	//bool tf2 = aprx_int(res);
	//return res;

	// => dn = log(f/fref)/log(gint^(1/ntet))
	//double gint_pow_ntet = std::pow(static_cast<double>(gint),1.0/static_cast<double>(ntet));
	//double ffref = frq_in/ref_frq;
	//double scd_approx = std::log(ffref)/std::log(gint_pow_ntet);
	//return scd_approx + m_shift_scd;
}

