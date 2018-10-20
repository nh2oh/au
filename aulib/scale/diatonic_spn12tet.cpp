#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include "scale.h"
#include "diatonic_spn12tet.h"
#include "spn12tet.h"
#include "..\types\cent_oct_t.h"
#include "..\types\ntl_t.h"
#include "..\types\frq_t.h"
#include "..\types\scd_t.h"
#include "..\util\au_util_all.h"
#include "..\util\au_error.h"
/*
diatonic_spn12tet::diatonic_spn12tet() {
	build_sc(m_sc_base,"C"_ntl,ionian);
}*/
diatonic_spn12tet::diatonic_spn12tet(ntl_t base_ntl, mode m) {
	build_sc(m_sc_base,base_ntl,ionian);
}
diatonic_spn12tet::diatonic_spn12tet(spn12tet sc, ntl_t base_ntl, mode m) {
	build_sc(sc,base_ntl,ionian);
}

void diatonic_spn12tet::build_sc(spn12tet sc_base, ntl_t ntl_base, mode m) {
	m_sc_base = sc_base;
	m_ntl_base = ntl_base;
	m_shift_scd = m_sc_base.to_scd(ntstr_t{ntl_base,0}).to_int();
	m_mode_idx = static_cast<int>(m);
	
	scd_t curr_scd {m_shift_scd};
	for (int i=0; i<7; ++i) {
		if (i>0) {
			curr_scd += scd_t{m_ip[(i-1+m_mode_idx)%m_ip.size()]};
		}
		m_ntls.push_back(ntl_t{m_sc_base.to_ntstr(curr_scd)});
	}

	m_name = "Diatonic scale " + m_ntl_base.print() + " ...";
	m_description = "Constructed from spn12tet:  \n";
	m_description += m_sc_base.name() + "\n" + m_sc_base.description();
}

// Going to route all conversion functions into m_sc_base using this function
// to get the scd.  
// TODO:  Come up w/ a better way to convert the diatonic scd to the chromatic
// scd.  
scd_t diatonic_spn12tet::scd_diatonic2spn12tet(scd_t scd_in) const {
	rscdoctn_t rscd {scd_in,7};
	return m_sc_base.to_scd(ntstr_t{m_ntls[rscd.to_int()],octn_t{scd_in,7}});
	// Alternatively, could use the m_ntls vector to convert to an ntstr...
}
scd_t diatonic_spn12tet::scd_spn12tet2diatonic(scd_t scd_in) const {
	ntstr_t ntstr = m_sc_base.to_ntstr(scd_in);
	ntl_t ntl {ntstr};
	auto idx = std::find(m_ntls.begin(), m_ntls.end(), ntl)-m_ntls.begin();
	return scd_t {static_cast<int>(idx)};
}

int diatonic_spn12tet::n() const {
	return 7;
}
std::string diatonic_spn12tet::name() const {
	return m_name;
}
std::string diatonic_spn12tet::description() const {
	return m_description;
}

ntstr_t diatonic_spn12tet::to_ntstr(scd_t scd_in) const {
	scd_t scd = diatonic_spn12tet::scd_diatonic2spn12tet(scd_in);
	return m_sc_base.to_ntstr(scd);
}
std::vector<ntstr_t> diatonic_spn12tet::to_ntstr(const std::vector<scd_t>& scds) const {
	std::vector<ntstr_t> ntstrs(scds.size(),ntstr_t{});
	for (int i=0; i<scds.size(); ++i) {
		ntstrs[i] = to_ntstr(scds[i]);
	}
	return ntstrs;
}
ntstr_t diatonic_spn12tet::to_ntstr(frq_t frq) const {
	au_assert(isinsc(frq),"frq not in scale");
	return m_sc_base.to_ntstr(frq);
}
std::vector<ntstr_t> diatonic_spn12tet::to_ntstr(const std::vector<frq_t>& frqs) const {
	std::vector<ntstr_t> ntstrs(frqs.size(),ntstr_t{});
	for (int i=0; i<frqs.size(); ++i) {
		ntstrs[i] = to_ntstr(frqs[i]);
	}
	return ntstrs;
}
frq_t diatonic_spn12tet::to_frq(scd_t scd_in) const {
	scd_t scd = diatonic_spn12tet::scd_diatonic2spn12tet(scd_in);
	return m_sc_base.to_frq(scd_in);
}
std::vector<frq_t> diatonic_spn12tet::to_frq(const std::vector<scd_t>& scds) const {
	std::vector<frq_t> frqs(scds.size(),frq_t{});
	for (int i=0; i<scds.size(); ++i) {
		frqs[i] = to_frq(scds[i]);
	}
	return frqs;
}
frq_t diatonic_spn12tet::to_frq(ntstr_t ntstr) const {
	au_assert(isinsc(ntl_t{ntstr}),"ntstr not in scale");
	return m_sc_base.to_frq(ntstr);
}
std::vector<frq_t> diatonic_spn12tet::to_frq(const std::vector<ntstr_t>& ntstrs) const {
	std::vector<frq_t> frqs(ntstrs.size(),frq_t{});
	for (int i=0; i<ntstrs.size(); ++i) {
		frqs[i] = to_frq(ntstrs[i]);
	}
	return frqs;
}

scd_t diatonic_spn12tet::to_scd(frq_t frq) const {
	au_assert(isinsc(frq),"frq not in scale");
	scd_t scd_12tet = m_sc_base.to_scd(frq);
	return scd_spn12tet2diatonic(scd_12tet);
}
std::vector<scd_t> diatonic_spn12tet::to_scd(const std::vector<frq_t>& frqs) const {
	std::vector<scd_t> scds(frqs.size(),scd_t{});
	for (int i=0; i<frqs.size(); ++i) {
		scds[i] = to_scd(frqs[i]);
	}
	return scds;
}
scd_t diatonic_spn12tet::to_scd(ntstr_t ntstr) const {
	au_assert(isinsc(ntl_t{ntstr}),"ntstr not in scale");
	scd_t scd_12tet = m_sc_base.to_scd(ntstr);
	return scd_spn12tet2diatonic(scd_12tet);
}
std::vector<scd_t> diatonic_spn12tet::to_scd(const std::vector<ntstr_t>& ntstrs) const {
	std::vector<scd_t> scds(ntstrs.size(),scd_t{});
	for (int i=0; i<ntstrs.size(); ++i) {
		scds[i] = to_scd(ntstrs[i]);
	}
	return scds;
}

octn_t diatonic_spn12tet::to_octn(scd_t scd_in) const {
	scd_t scd_12tet = scd_diatonic2spn12tet(scd_in);
	return m_sc_base.to_octn(scd_12tet);
}
octn_t diatonic_spn12tet::to_octn(frq_t frq) const {
	au_assert(isinsc(frq),"frq not in scale");
	return m_sc_base.to_octn(frq);
}

bool diatonic_spn12tet::isinsc(frq_t frq) const {
	ntstr_t ntstr = m_sc_base.to_ntstr(frq);
	return isinsc(ntl_t{ntstr});
}
bool diatonic_spn12tet::isinsc(ntl_t ntl) const {
	return std::find(m_ntls.begin(),m_ntls.end(),ntl) != m_ntls.end();
}
std::string diatonic_spn12tet::print(scd_t from, scd_t to) const {
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

