#include "scale.h"
#include "diatonic_spn12tet.h"
#include "spn12tet.h"
#include "..\types\ntl_t.h"
#include "..\types\frq_t.h"
//#include "..\types\scd_t.h"
//#include "..\util\au_util_all.h"
//#include "..\util\au_error.h"
#include <string>
#include <vector>
#include <algorithm>  // std::find() (isinsc(ntl_t))
#include <exception>  // std::abort()

diatonic_spn12tet::diatonic_spn12tet(ntl_t base_ntl, mode m) {
	build_sc(m_sc_base,base_ntl,m);
}
diatonic_spn12tet::diatonic_spn12tet(spn12tet sc, ntl_t base_ntl, mode m) {
	build_sc(sc,base_ntl,m);
}

void diatonic_spn12tet::build_sc(spn12tet sc_base, ntl_t ntl_base, mode m) {
	m_sc_base = sc_base;
	m_ntl_base = ntl_base;
	m_shift_scd = m_sc_base.to_scd(ntl_base,octn_t{0}) - m_sc_base.to_scd(0);
	m_mode_idx = static_cast<int>(m);
	
	spn12tet::scd3_t curr_spn_scd {m_sc_base.to_scd(m_shift_scd)};
	for (int i=0; i<7; ++i) {
		if (i>0) {   // TODO:  What's w/ this condition???
			// curr_spn_scd += scd_t{m_ip[(i-1+m_mode_idx)%m_ip.size()]};  // TODO:  Replace w/ ring
			curr_spn_scd += m_ip[(i-1+m_mode_idx)%m_ip.size()];  // TODO:  Replace w/ ring
		}
		//m_ntls[i] = m_sc_base.to_ntstr(curr_scd).ntl();
		m_ntls[i] = (*curr_spn_scd).ntl;
	}

	m_name = "Diatonic scale " + m_ntl_base.print() + " ...";
	m_description = "Constructed from spn12tet:  \n";
	m_description += m_sc_base.name() + "\n" + m_sc_base.description();
}

std::string diatonic_spn12tet::name() const {
	return m_name;
}
std::string diatonic_spn12tet::description() const {
	return m_description;
}
std::string diatonic_spn12tet::print() const {
	return print(0,12);
}
std::string diatonic_spn12tet::print(int from, int to) const {
	return m_sc_base.print(scd_diatonic2spn12tet(to_scd(from))-m_sc_base.to_scd(0),
		scd_diatonic2spn12tet(to_scd(to))-m_sc_base.to_scd(0));
}


spn12tet::scd3_t diatonic_spn12tet::scd_diatonic2spn12tet(const diatonic_spn12tet::scd3_t& scd_d) const {
	int n_scd_spn = (scd_d - this->to_scd(0)) + m_shift_scd;
	return m_sc_base.to_scd(n_scd_spn);
}
diatonic_spn12tet::scd3_t diatonic_spn12tet::scd_spn12tet2diatonic(const spn12tet::scd3_t& scd_spn) const {
	int n_scd_d = (scd_spn - m_sc_base.to_scd(0)) - m_shift_scd;
	return this->to_scd(n_scd_d);
}

diatonic_spn12tet::scd3_t diatonic_spn12tet::to_scd(const int& i) const {
	return diatonic_spn12tet::scd3_t {i,this};
}
diatonic_spn12tet::scd3_t diatonic_spn12tet::to_scd(const ntl_t& ntl, const octn_t& o) const {
	if (!isinsc(ntl)) {
		std::abort();
	}
	return scd_spn12tet2diatonic(m_sc_base.to_scd(ntl,o));
}
diatonic_spn12tet::scd3_t diatonic_spn12tet::to_scd(const note_t& nt) const {
	if (!isinsc(nt.ntl)) {
		std::abort();
	}
	spn12tet::scd3_t scd_12tet = m_sc_base.to_scd(nt);
	return scd_spn12tet2diatonic(scd_12tet);
}
std::vector<diatonic_spn12tet::scd3_t> diatonic_spn12tet::to_scd(const std::vector<note_t>& nts) const {
	std::vector<diatonic_spn12tet::scd3_t> scds {};  scds.reserve(nts.size());
	for (const auto& e: nts) {
		scds.push_back(to_scd(e));
	}
	return scds;
}
diatonic_spn12tet::scd3_t diatonic_spn12tet::to_scd(const frq_t& frq) const {
	if (!isinsc(frq)) {
		std::abort();
	}
	return scd_spn12tet2diatonic(m_sc_base.to_scd(frq));
}
std::vector<diatonic_spn12tet::scd3_t> diatonic_spn12tet::to_scd(const std::vector<frq_t>& frqs) const {
	std::vector<diatonic_spn12tet::scd3_t> scds {};  scds.reserve(frqs.size());
	for (const auto& e : frqs) {
		scds.push_back(to_scd(e));
	}
	return scds;
}

// Getter called by diatonic_spn12tet::scd3_t::operator*()
note_t diatonic_spn12tet::to_note(int scd_idx) const {
	diatonic_spn12tet::scd3_t scd_d = to_scd(scd_idx);
	spn12tet::scd3_t scd_spn = scd_diatonic2spn12tet(scd_d);

	return *scd_spn;
}

diatonic_spn12tet::scd3_t::scd3_t(int i, const diatonic_spn12tet *sc) {
	m_val = i;
	m_sc = sc;
}
note_t diatonic_spn12tet::scd3_t::operator*() const {
	return m_sc->to_note(m_val);
}
diatonic_spn12tet::scd3_t& diatonic_spn12tet::scd3_t::operator++() {  // prefix
	m_val++;
	return *this;
}
spn12tet::scd3_t spn12tet::scd3_t::operator++(int) {  // postfix
	spn12tet::scd3_t scd = *this; ++*this;
	return scd;
}
diatonic_spn12tet::scd3_t& diatonic_spn12tet::scd3_t::operator--() { // prefix
	m_val--;
	return *this;
}
diatonic_spn12tet::scd3_t diatonic_spn12tet::scd3_t::operator--(int) {  // postfix
	diatonic_spn12tet::scd3_t scd = *this; --*this;
	return scd;
}
diatonic_spn12tet::scd3_t& diatonic_spn12tet::scd3_t::operator-=(const scd3_t& rhs) {
	if (m_sc != rhs.m_sc) { std::abort(); }
	m_val-=rhs.m_val;
	return *this;
}
diatonic_spn12tet::scd3_t& diatonic_spn12tet::scd3_t::operator-=(const int& rhs) {
	m_val+=rhs;
	return *this;
}
diatonic_spn12tet::scd3_t& diatonic_spn12tet::scd3_t::operator+=(const int& rhs) {
	m_val-=rhs;
	return *this;
}
int operator-(const diatonic_spn12tet::scd3_t& lhs, const diatonic_spn12tet::scd3_t& rhs) {
	if (lhs.m_sc != rhs.m_sc) { std::abort(); }
	return (lhs.m_val - rhs.m_val);
}


bool diatonic_spn12tet::isinsc(const frq_t& frq) const {
	if (m_sc_base.isinsc(frq)) {
		return isinsc((*m_sc_base.to_scd(frq)).ntl);
	}
	return false;
}
bool diatonic_spn12tet::isinsc(const ntl_t& ntl) const {
	return std::find(m_ntls.begin(),m_ntls.end(),ntl) != m_ntls.end();
}

