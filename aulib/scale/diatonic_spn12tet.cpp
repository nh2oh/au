#include "scale.h"
#include "diatonic_spn12tet.h"
#include "spn12tet.h"
#include "..\types\ntl_t.h"
#include "..\types\frq_t.h"
//#include "..\types\scd_t.h"
//#include "..\util\au_util_all.h"
//#include "..\util\au_error.h"
#include "..\util\au_util.h"  // bsprintf()
#include <string>
#include <vector>
#include <algorithm>  // std::find() (isinsc(ntl_t))
#include <exception>  // std::abort()
#include <cmath>  // std::floor()

diatonic_spn12tet::diatonic_spn12tet(ntl_t base_ntl, mode m) {
	build_sc(m_sc_base,base_ntl,m);
}
diatonic_spn12tet::diatonic_spn12tet(spn12tet sc, ntl_t base_ntl, mode m) {
	build_sc(sc,base_ntl,m);
}

void diatonic_spn12tet::build_sc(spn12tet sc_base, ntl_t ntl_base, mode m) {
	if (!isinsc(ntl_base)) { std::abort(); }
	m_sc_base = sc_base;
	//m_ntl_base = ntl_base;
	//m_shift_scd = m_sc_base.to_scd(ntl_base,octn_t{0}) - m_sc_base.to_scd(0);
	m_shift_basentl = std::find(m_ntls.begin(),m_ntls.end(),ntl_base) - m_ntls.begin();
	m_mode_idx = static_cast<int>(m);
	/*
	spn12tet::scd3_t curr_spn_scd {m_sc_base.to_scd(m_shift_scd)};
	for (int i=0; i<7; ++i) {
		if (i>0) {   // TODO:  What's w/ this condition???
			// curr_spn_scd += scd_t{m_ip[(i-1+m_mode_idx)%m_ip.size()]};  // TODO:  Replace w/ ring
			curr_spn_scd += m_ip[(i-1+m_mode_idx)%m_ip.size()];  // TODO:  Replace w/ ring
		}
		//m_ntls[i] = m_sc_base.to_ntstr(curr_scd).ntl();
		m_ntls[i] = (*curr_spn_scd).ntl;
	}*/

	m_name = "Diatonic scale " + m_ntl_base.print() + " ...";
	m_description = "Constructed from spn12tet:  \n";
	m_description += m_sc_base.name() + "\n" + m_sc_base.description();
}



diatonic_spn12tet::base_ntl_idx_t diatonic_spn12tet::base_ntl_idx(const ntl_t& ntl, const octn_t& o) const {
	base_ntl_idx_t res {};
	auto it = std::find(m_ntls.begin(),m_ntls.end(),ntl);
	if (it == m_ntls.end()) {
		res.is_valid = false;
		return res;
	}
	res.is_valid = true;

	res.ntl_idx = static_cast<int>(it-m_ntls.begin());  //(((idx_d_shift)%m_n)+m_n)%m_n;
	//res.oct = o.to_int(); //static_cast<int>(std::floor(static_cast<double>(idx_d_shift)/static_cast<double>(m_n)));
	//res.spn_idx = m_sc_base.to_scd(m_ntls[res.ntl_idx],octn_t{res.oct}) - m_sc_base.to_scd(0);
	res.scd_idx = res.ntl_idx + (o.to_int())*m_n; //m_sc_base.to_scd(ntl,o)-m_sc_base.to_scd(0);
}
diatonic_spn12tet::base_ntl_idx_t diatonic_spn12tet::base_ntl_idx(const frq_t& frq) const {
	note_t spn_nt = *m_sc_base.to_scd(frq);
	return base_ntl_idx(spn_nt.ntl,spn_nt.oct);
}


/*
// An spn12tet scd
diatonic_spn12tet::base_ntl_idx_t diatonic_spn12tet::base_ntl_idx(spn12tet::scd3_t scd_spn) const {
	base_ntl_idx_t res {};
	auto nt = *scd_spn;
	res.spn_idx = scd_spn - m_sc_base.to_scd(0);
	int idx_d_shift = idx_d+m_shift_basentl;
	res.ntl_idx = (((idx_d_shift)%m_n)+m_n)%m_n;
	res.oct = static_cast<int>(std::floor(static_cast<double>(idx_d_shift)/static_cast<double>(m_n)));
}*/

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
	std::string s {};
	s = dbk::bsprintf("%s\n%s\n\n",m_name,m_description);
	
	for (int i=from; i<to; ++i) {
		diatonic_spn12tet::scd3_t curr_scd {i,this};
		note_t curr_note = *curr_scd;

		s += dbk::bsprintf("%d:\t%s\t%.3f\n",
			i,curr_note.ntl.print(),(curr_note.frq/frq_t{1}));
	}
	s += "\n\n";

	return s;
	/*return m_sc_base.print(scd_diatonic2spn12tet(to_scd(from))-m_sc_base.to_scd(0),
		scd_diatonic2spn12tet(to_scd(to))-m_sc_base.to_scd(0));*/
}

/*
spn12tet::scd3_t diatonic_spn12tet::scd_diatonic2spn12tet(const diatonic_spn12tet::scd3_t& scd_d) const {
	int n_scd_spn = (scd_d - this->to_scd(0)) + m_shift_scd;
	return m_sc_base.to_scd(n_scd_spn);
}
diatonic_spn12tet::scd3_t diatonic_spn12tet::scd_spn12tet2diatonic(const spn12tet::scd3_t& scd_spn) const {
	int n_scd_d = (scd_spn - m_sc_base.to_scd(0)) - m_shift_scd;
	return this->to_scd(n_scd_d);
}
*/
diatonic_spn12tet::scd3_t diatonic_spn12tet::to_scd(const int& i) const {
	return diatonic_spn12tet::scd3_t {i,this};
}
diatonic_spn12tet::scd3_t diatonic_spn12tet::to_scd(const ntl_t& ntl, const octn_t& o) const {
	auto res = base_ntl_idx(ntl,o);
	if (!res.is_valid) {
		std::abort();
	}
	//return scd_spn12tet2diatonic(m_sc_base.to_scd(ntl,o));
	return diatonic_spn12tet::scd3_t {res.scd_idx,this};
}
diatonic_spn12tet::scd3_t diatonic_spn12tet::to_scd(const note_t& nt) const {
	auto res = base_ntl_idx(nt.ntl,nt.oct);
	if (!res.is_valid) {  // TODO:  Also check the frq
		std::abort();
	}
	//return scd_spn12tet2diatonic(m_sc_base.to_scd(ntl,o));
	return diatonic_spn12tet::scd3_t {res.scd_idx,this};
	/*if (!isinsc(nt.ntl)) {
		std::abort();
	}
	spn12tet::scd3_t scd_12tet = m_sc_base.to_scd(nt);
	return scd_spn12tet2diatonic(scd_12tet);*/
}
std::vector<diatonic_spn12tet::scd3_t> diatonic_spn12tet::to_scd(const std::vector<note_t>& nts) const {
	std::vector<diatonic_spn12tet::scd3_t> scds {};  scds.reserve(nts.size());
	for (const auto& e: nts) {
		scds.push_back(to_scd(e));
	}
	return scds;
}
diatonic_spn12tet::scd3_t diatonic_spn12tet::to_scd(const frq_t& frq) const {
	auto res = base_ntl_idx(frq);
	if (!res.is_valid) {
		std::abort();
	}
	//return scd_spn12tet2diatonic(m_sc_base.to_scd(ntl,o));
	return diatonic_spn12tet::scd3_t {res.scd_idx,this};
	/*if (!isinsc(frq)) {
		std::abort();
	}
	return scd_spn12tet2diatonic(m_sc_base.to_scd(frq));*/
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
	int ntl_idx = ((scd_idx%m_n)+m_n)%m_n-m_shift_basentl;
	octn_t octn {static_cast<int>(std::floor(static_cast<double>(scd_idx)/static_cast<double>(m_n)))};
		// NB:  Computed from the ntl_idx _unshifted_  ????
		// ...This is what spn12tet does
	frq_t frq = (*m_sc_base.to_scd(m_ntls[ntl_idx],octn)).frq;
	
	return note_t {m_ntls[ntl_idx],octn,frq};
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
diatonic_spn12tet::scd3_t diatonic_spn12tet::scd3_t::operator++(int) {  // postfix
	diatonic_spn12tet::scd3_t scd = *this; ++*this;
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
	base_ntl_idx_t idx = base_ntl_idx(frq);
	return idx.is_valid;
	/*if (m_sc_base.isinsc(frq)) {
		return isinsc((*m_sc_base.to_scd(frq)).ntl);
	}
	return false;*/
}
bool diatonic_spn12tet::isinsc(const ntl_t& ntl) const {
	base_ntl_idx_t idx = base_ntl_idx(ntl,octn_t{0});
	return idx.is_valid;
	//return std::find(m_ntls.begin(),m_ntls.end(),ntl) != m_ntls.end();
}
bool diatonic_spn12tet::isinsc(const note_t& note) const {
	// Both the ntl and frq must be valid, AND the ntl => by the frq must be the same.  
	base_ntl_idx_t idx_ntloct = base_ntl_idx(note.ntl,note.oct);
	if (!idx_ntloct.is_valid) { return false; }
	base_ntl_idx_t idx_frq = base_ntl_idx(note.frq);
	if (!idx_frq.is_valid) { return false; }

	return (idx_ntloct.scd_idx == idx_frq.scd_idx);
}


