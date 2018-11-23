#include "scale.h"
#include "diatonic_spn.h"
#include "spn.h"
#include "..\types\ntl_t.h"
#include "..\types\frq_t.h"
#include "..\util\au_util.h"  // bsprintf()
#include <string>
#include <vector>
#include <algorithm>  // std::find() (isinsc(ntl_t))
#include <exception>  // std::abort()
#include <cmath>  // std::floor()

diatonic_spn::diatonic_spn(ntl_t base_ntl, mode m) {
	build_sc(m_sc_base,base_ntl,m);
}
diatonic_spn::diatonic_spn(spn sc, ntl_t base_ntl, mode m) {
	build_sc(sc,base_ntl,m);
}

void diatonic_spn::build_sc(spn sc_base, ntl_t ntl_base, mode m) {
	m_sc_base = sc_base;
	if (!m_sc_base.isinsc(ntl_base)) { std::abort(); }
	
	m_mode_idx = static_cast<int>(m);
	spn::scd3_t spn_scd = m_sc_base.to_scd(ntl_base,octn_t{0});
	for (int i=0; i<m_n; ++i) {
		if (i>0) { 
			// Note this weird i>0 condition:  The first ntl (idx==0) is just ntl_base;
			// adding m_ip[0] to the spn_scd corresponding to ntl_base yields the _second_
			// ntl of the scale (m_ntls[1]).  
			spn_scd += m_ip[(i-1+m_mode_idx)%m_ip.size()];  // TODO:  Replace w/ ring
		}
		m_ntls[i] = (*spn_scd).ntl;
	}

	// Always 0, since i am rewriting the m_ntls vector in the loop above
	m_shift_basentl = 0;  // TODO:  Therefore i don't need it...

	m_name = "Diatonic scale " + m_ntl_base.print() + " ...";
	m_description = "Constructed from spn:  \n";
	m_description += m_sc_base.name() + "\n" + m_sc_base.description();
}

diatonic_spn::base_ntl_idx_t diatonic_spn::base_ntl_idx(const ntl_t& ntl, const octn_t& o) const {
	base_ntl_idx_t res {};
	auto it = std::find(m_ntls.begin(),m_ntls.end(),ntl);
	if (it == m_ntls.end()) {
		res.is_valid = false;
		return res;
	}
	res.is_valid = true;

	res.ntl_idx = static_cast<int>(it-m_ntls.begin());
	res.scd_idx = res.ntl_idx + (o.to_int())*m_n;
}
diatonic_spn::base_ntl_idx_t diatonic_spn::base_ntl_idx(const frq_t& frq) const {
	note_t spn_nt = *m_sc_base.to_scd(frq);
	return base_ntl_idx(spn_nt.ntl,spn_nt.oct);
}

std::string diatonic_spn::name() const {
	return m_name;
}
std::string diatonic_spn::description() const {
	return m_description;
}
std::string diatonic_spn::print() const {
	return print(0,12);
}
std::string diatonic_spn::print(int from, int to) const {
	std::string s {};
	s = dbk::bsprintf("%s\n%s\n\n",m_name,m_description);
	
	for (int i=from; i<to; ++i) {
		diatonic_spn::scd3_t curr_scd {i,this};
		note_t curr_note = *curr_scd;

		s += dbk::bsprintf("%d:\t%s\t%.3f\n",
			i,curr_note.ntl.print(),(curr_note.frq/frq_t{1}));
	}
	s += "\n\n";

	return s;
}


diatonic_spn::scd3_t diatonic_spn::to_scd(const int& i) const {
	return diatonic_spn::scd3_t {i,this};
}
diatonic_spn::scd3_t diatonic_spn::to_scd(const ntl_t& ntl, const octn_t& o) const {
	auto res = base_ntl_idx(ntl,o);
	if (!res.is_valid) {
		std::abort();
	}

	return diatonic_spn::scd3_t {res.scd_idx,this};
}
diatonic_spn::scd3_t diatonic_spn::to_scd(const note_t& nt) const {
	auto res = base_ntl_idx(nt.ntl,nt.oct);
	if (!res.is_valid) {  // TODO:  Also check the frq
		std::abort();
	}

	return diatonic_spn::scd3_t {res.scd_idx,this};
}
std::vector<diatonic_spn::scd3_t> diatonic_spn::to_scd(const std::vector<note_t>& nts) const {
	std::vector<diatonic_spn::scd3_t> scds {};  scds.reserve(nts.size());
	for (const auto& e: nts) {
		scds.push_back(to_scd(e));
	}
	return scds;
}
diatonic_spn::scd3_t diatonic_spn::to_scd(const frq_t& frq) const {
	auto res = base_ntl_idx(frq);
	if (!res.is_valid) {
		std::abort();
	}

	return diatonic_spn::scd3_t {res.scd_idx,this};
}
std::vector<diatonic_spn::scd3_t> diatonic_spn::to_scd(const std::vector<frq_t>& frqs) const {
	std::vector<diatonic_spn::scd3_t> scds {};  scds.reserve(frqs.size());
	for (const auto& e : frqs) {
		scds.push_back(to_scd(e));
	}
	return scds;
}

// Getter called by diatonic_spn::scd3_t::operator*()
note_t diatonic_spn::to_note(int scd_idx) const {
	int ntl_idx = ((scd_idx%m_n)+m_n)%m_n-m_shift_basentl;
	octn_t octn {static_cast<int>(std::floor(static_cast<double>(scd_idx)/static_cast<double>(m_n)))};
	frq_t frq = (*m_sc_base.to_scd(m_ntls[ntl_idx],octn)).frq;
	
	return note_t {m_ntls[ntl_idx],octn,frq};
}

diatonic_spn::scd3_t::scd3_t(int i, const diatonic_spn *sc) {
	m_val = i;
	m_sc = sc;
}
note_t diatonic_spn::scd3_t::operator*() const {
	return m_sc->to_note(m_val);
}
diatonic_spn::scd3_t& diatonic_spn::scd3_t::operator++() {  // prefix
	m_val++;
	return *this;
}
diatonic_spn::scd3_t diatonic_spn::scd3_t::operator++(int) {  // postfix
	diatonic_spn::scd3_t scd = *this; ++*this;
	return scd;
}
diatonic_spn::scd3_t& diatonic_spn::scd3_t::operator--() { // prefix
	m_val--;
	return *this;
}
diatonic_spn::scd3_t diatonic_spn::scd3_t::operator--(int) {  // postfix
	diatonic_spn::scd3_t scd = *this; --*this;
	return scd;
}
diatonic_spn::scd3_t& diatonic_spn::scd3_t::operator-=(const scd3_t& rhs) {
	if (m_sc != rhs.m_sc) { std::abort(); }
	m_val-=rhs.m_val;
	return *this;
}
diatonic_spn::scd3_t& diatonic_spn::scd3_t::operator-=(const int& rhs) {
	m_val+=rhs;
	return *this;
}
diatonic_spn::scd3_t& diatonic_spn::scd3_t::operator+=(const int& rhs) {
	m_val+=rhs;
	return *this;
}
int operator-(const diatonic_spn::scd3_t& lhs, const diatonic_spn::scd3_t& rhs) {
	if (lhs.m_sc != rhs.m_sc) { std::abort(); }
	return (lhs.m_val - rhs.m_val);
}


bool diatonic_spn::isinsc(const frq_t& frq) const {
	base_ntl_idx_t idx = base_ntl_idx(frq);
	return idx.is_valid;
}
bool diatonic_spn::isinsc(const ntl_t& ntl) const {
	base_ntl_idx_t idx = base_ntl_idx(ntl,octn_t{0});
	return idx.is_valid;
}
bool diatonic_spn::isinsc(const note_t& note) const {
	// Both the ntl and frq must be valid, AND the ntl => by the frq must be the same.  
	base_ntl_idx_t idx_ntloct = base_ntl_idx(note.ntl,note.oct);
	if (!idx_ntloct.is_valid) { return false; }
	base_ntl_idx_t idx_frq = base_ntl_idx(note.frq);
	if (!idx_frq.is_valid) { return false; }

	return (idx_ntloct.scd_idx == idx_frq.scd_idx);
}


