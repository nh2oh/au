#include "spn12tet3.h"
#include "scale.h"  // n_eqt(), etc
#include "..\util\au_algs_math.h"  // aprx_int(), aprx_eq()
#include "..\util\au_util.h"  // bsprintf()
#include "..\types\frq_t.h"
#include "..\types\ntl_t.h"
#include <algorithm>
#include <exception>  // std::abort()
#include <string>


spn12tet3::spn12tet3(pitch_std3 ps) {
	if (ps.gen_int <= 0 || ps.ntet <= 0) { std::abort(); }  // TODO:  Move into pitch_std class
	m_pstd = ps;

	auto it = std::find(m_ntls.begin(),m_ntls.end(),m_pstd.ref_note.ntl);
	if (it == m_ntls.end()) { std::abort(); }

	m_shift_scd = (m_pstd.ref_note.oct.to_int())*(spn12tet3::m_ntls.size()) + (it-m_ntls.begin());
	// Expect 57 for a ref pitch of A(4)
	auto x = (m_shift_scd == 57);
}

std::string spn12tet3::print() const {
	return spn12tet3::print(0,12);
}
std::string spn12tet3::print(int from, int to) const {
	std::string s {};
	s = dbk::bsprintf("%s\n%s\n\n",m_name,m_description);
	
	for (int i=from; i<to; ++i) {
		spn12tet3::scd3_t curr_scd {i,this};
		note_t curr_note = *curr_scd;

		s += dbk::bsprintf("%d:\t%s\t%.3f\n",
			i,curr_note.ntl.print(),(curr_note.frq/frq_t{1}));
	}
	s += "\n\n";

	return s;
}
std::string spn12tet3::name() const {
	return m_name;
}
std::string spn12tet3::description() const {
	return m_description;
}

bool spn12tet3::isinsc(const ntl_t& ntl) const {
	base_ntl_idx_t idx = base_ntl_idx(ntl,octn_t{0});
	return idx.is_valid;
}
bool spn12tet3::isinsc(const frq_t& frq_in) const {
	base_ntl_idx_t idx = base_ntl_idx(frq_in);
	return idx.is_valid;
}
bool spn12tet3::isinsc(const note_t& note_in) const {
	// Both the ntl and frq must be valid, AND the ntl => by the frq must be the same.  
	base_ntl_idx_t idx_ntloct = base_ntl_idx(note_in.ntl,note_in.oct);
	if (!idx_ntloct.is_valid) { return false; }
	base_ntl_idx_t idx_frq = base_ntl_idx(note_in.frq);
	if (!idx_frq.is_valid) { return false; }

	return (idx_ntloct.scd_idx == idx_frq.scd_idx);
}

spn12tet3::scd3_t spn12tet3::to_scd(const ntl_t& ntl, const octn_t& o) const {
	base_ntl_idx_t idx = base_ntl_idx(ntl,o);
	if (!idx.is_valid) { std::abort(); }
	return spn12tet3::scd3_t {idx.scd_idx,this};
}
spn12tet3::scd3_t spn12tet3::to_scd(const frq_t& frq) const {
	base_ntl_idx_t idx = base_ntl_idx(frq);
	if (!idx.is_valid) { std::abort(); }
	return  spn12tet3::scd3_t {idx.scd_idx,this};
}
spn12tet3::scd3_t spn12tet3::to_scd(const note_t& note) const {
	return to_scd(note.ntl,note.oct);
}
std::vector<spn12tet3::scd3_t> spn12tet3::to_scd(const std::vector<note_t>& nts) const {
	std::vector<spn12tet3::scd3_t> scds {}; scds.reserve(nts.size());
	for (const auto& e : nts) {
		scds.push_back(to_scd(e));
	}
	return scds;
}


// Private method
spn12tet3::base_ntl_idx_t spn12tet3::base_ntl_idx(const ntl_t& ntl, const octn_t& oct) const {
	base_ntl_idx_t res {};
	auto it = std::find(m_ntls.begin(),m_ntls.end(),ntl);
	
	res.is_valid = (it!=m_ntls.end());
	res.ntl_idx = static_cast<int>(it-m_ntls.begin());
	res.scd_idx = res.ntl_idx + (oct.to_int())*N;// + m_shift_scd;
	return res;
}

// Private method
spn12tet3::base_ntl_idx_t spn12tet3::base_ntl_idx(const frq_t& frq_in) const {
	base_ntl_idx_t res {};
	double idx = n_eqt(frq_in, m_pstd.ref_note.frq, m_pstd.ntet, m_pstd.gen_int);
	if (!aprx_int(idx)) {
		res.is_valid = false;
		return res;
	}

	res.scd_idx = static_cast<int>(std::round(idx)) + m_shift_scd;
	res.ntl_idx = ((res.scd_idx%N)+N)%N;
	//res.ntl_idx = static_cast<int>((res.scd_idx+m_ntls.size())%(m_ntls.size()));
	// TODO:  Danger here: m_ntls.size() is unsigned, res.scd may be - ...
	// I don't know the implicit conversion rules.  
	res.is_valid = true;
	return res;
}

// Getter called by spn12tet3::scd3_t::operator*()
note_t spn12tet3::to_note(int scd_idx) const {
	
	int ntl_idx = ((scd_idx%N)+N)%N;
	// int ntl_idx = (scd_idx+m_ntls.size())%(m_ntls.size());
	// int ntl_idx = ((scd_idx-m_shift_scd)+m_ntls.size())%(m_ntls.size());
	// TODO:  Danger here: m_ntls.size() is unsigned, res.scd may be - ...
	// I don't know the implicit conversion rules.  

	frq_t frq = frq_eqt(scd_idx-m_shift_scd,m_pstd.ref_note.frq,m_pstd.ntet,m_pstd.gen_int);
	
	octn_t octn {static_cast<int>(std::floor(static_cast<double>(scd_idx)/static_cast<double>(N)))};
	// octn_t octn {static_cast<int>((scd_idx+m_ntls.size())%(m_ntls.size()))};
	// TODO:  Danger here: m_ntls.size() is unsigned, res.scd may be - ...
	// I don't know the implicit conversion rules.  

	return note_t {m_ntls[ntl_idx],octn,frq};
}








spn12tet3::scd3_t::scd3_t(int i, const spn12tet3 *sc) {
	m_val = i;
	m_sc = sc;
}

note_t spn12tet3::scd3_t::operator*() const {
	return m_sc->to_note(m_val);
}

spn12tet3::scd3_t& spn12tet3::scd3_t::operator++() {  // prefix
	m_val++;
	return *this;
}
spn12tet3::scd3_t spn12tet3::scd3_t::operator++(int) {  // postfix
	spn12tet3::scd3_t scd = *this; ++*this;
	return scd;
}
spn12tet3::scd3_t& spn12tet3::scd3_t::operator--() { // prefix
	m_val--;
	return *this;
}
spn12tet3::scd3_t spn12tet3::scd3_t::operator--(int) {  // postfix
	spn12tet3::scd3_t scd = *this; --*this;
	return scd;
}
spn12tet3::scd3_t& spn12tet3::scd3_t::operator-=(const scd3_t& rhs) {
	if (m_sc != rhs.m_sc) { std::abort(); }
	m_val-=rhs.m_val;
	return *this;
}

int operator-(const spn12tet3::scd3_t& lhs, const spn12tet3::scd3_t& rhs) {
	if (lhs.m_sc != rhs.m_sc) { std::abort(); }
	return (lhs.m_val - rhs.m_val);
}


