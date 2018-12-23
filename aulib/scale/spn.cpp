#include "spn.h"
#include "scale.h"  // n_eqt(), etc
#include "dbklib\math.h"  // aprx_int()
#include "..\util\au_util.h"  // bsprintf()
#include "..\types\frq_t.h"
#include "..\types\ntl_t.h"
#include <algorithm>
#include <exception>  // std::abort()
#include <string>


spn::spn(pitch_std ps) {
	if (ps.gen_int <= 0 || ps.ntet <= 0) { std::abort(); }  // TODO:  Move into pitch_std class?
	m_pstd = ps;

	auto it = std::find(m_ntls.begin(),m_ntls.end(),m_pstd.ref_note.ntl);
	if (it == m_ntls.end()) { std::abort(); }

	m_shift_scd = (m_pstd.ref_note.oct.to_int())*(spn::m_ntls.size()) + (it-m_ntls.begin());
	// Expect 57 for a ref pitch of A(4)
}

std::string spn::print() const {
	return spn::print(0,12);
}
std::string spn::print(int from, int to) const {
	std::string s {};
	s = dbk::bsprintf("%s\n%s\n\n",m_name,m_description);
	
	for (int i=from; i<to; ++i) {
		note_t curr_note = this->operator[](i);

		s += dbk::bsprintf("%d:\t%s\t%.3f\n",
			i,curr_note.ntl.print(),(curr_note.frq/frq_t{1}));
	}
	s += "\n\n";

	return s;
}
std::string spn::name() const {
	return m_name;
}
std::string spn::description() const {
	return m_description;
}

bool spn::isinsc(const ntl_t& ntl) const {
	base_ntl_idx_t idx = base_ntl_idx(ntl,octn_t{0});
	return idx.is_valid;
}
bool spn::isinsc(const frq_t& frq_in) const {
	base_ntl_idx_t idx = base_ntl_idx(frq_in);
	return idx.is_valid;
}
bool spn::isinsc(const note_t& note_in) const {
	// Both the ntl and frq must be valid, AND the ntl => by the frq must be the same.  
	base_ntl_idx_t idx_ntloct = base_ntl_idx(note_in.ntl,note_in.oct);
	if (!idx_ntloct.is_valid) { return false; }
	base_ntl_idx_t idx_frq = base_ntl_idx(note_in.frq);
	if (!idx_frq.is_valid) { return false; }

	return (idx_ntloct.scd_idx == idx_frq.scd_idx);
}

int spn::to_scd(const ntl_t& ntl, const octn_t& o) const {
	base_ntl_idx_t idx = base_ntl_idx(ntl,o);
	if (!idx.is_valid) { std::abort(); }
	return idx.scd_idx;
}
int spn::to_scd(const frq_t& frq) const {
	base_ntl_idx_t idx = base_ntl_idx(frq);
	if (!idx.is_valid) { std::abort(); }
	return idx.scd_idx;
}

int spn::to_scd(const note_t& note) const {
	return to_scd(note.ntl,note.oct);
}
std::vector<int> spn::to_scd(const std::vector<note_t>& nts) const {
	std::vector<int> scds {}; scds.reserve(nts.size());
	for (const auto& e : nts) {
		scds.push_back(to_scd(e));
	}
	return scds;
}

// Private method
spn::base_ntl_idx_t spn::base_ntl_idx(const ntl_t& ntl, const octn_t& oct) const {
	base_ntl_idx_t res {};
	auto it = std::find(m_ntls.begin(),m_ntls.end(),ntl);
	
	res.is_valid = (it!=m_ntls.end());
	res.ntl_idx = static_cast<int>(it-m_ntls.begin());
	res.scd_idx = res.ntl_idx + (oct.to_int())*N;
	return res;
}

// Private method
spn::base_ntl_idx_t spn::base_ntl_idx(const frq_t& frq_in) const {
	base_ntl_idx_t res {};
	double idx = n_eqt(frq_in, m_pstd.ref_note.frq, m_pstd.ntet, m_pstd.gen_int);
	if (!aprx_int(idx)) {
		res.is_valid = false;
		return res;
	}

	res.scd_idx = static_cast<int>(std::round(idx)) + m_shift_scd;
	res.ntl_idx = ((res.scd_idx%N)+N)%N;
	res.is_valid = true;
	return res;
}

note_t spn::operator[](int i) const {
	int ntl_idx = ((i%N)+N)%N;
	frq_t frq = frq_eqt(i-m_shift_scd,m_pstd.ref_note.frq,m_pstd.ntet,m_pstd.gen_int);
	octn_t octn {static_cast<int>(std::floor(static_cast<double>(i)/static_cast<double>(N)))};
	
	return note_t {m_ntls[ntl_idx],octn,frq};
}
spn::iterator spn::zero() const {
	return spn::iterator {0,this};
}


