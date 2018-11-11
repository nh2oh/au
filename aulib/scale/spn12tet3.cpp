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
}

std::string spn12tet3::print() const {
	return spn12tet3::print(spn12tet3::scd3_t {0}, spn12tet3::scd3_t {spn12tet3::m_ntls.size()});
}
std::string spn12tet3::print(spn12tet3::scd3_t from, spn12tet3::scd3_t to) const {
	std::string s {};
	s = dbk::bsprintf("%s\n%s\n\n",m_name,m_description);
	
	spn12tet3::scd3_t zero {0};
	for (spn12tet3::scd3_t curr_scd=from; curr_scd<to; ++curr_scd) {
		s += dbk::bsprintf("%d:\t%s\t%.3f\n",
			(curr_scd-zero),(*curr_scd).print(),(*curr_scd).frq);
		// TODO:  A note_t has no print() method...
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
	base_ntl_idx_t idx = base_ntl_idx(ntl);
	return idx.is_valid;
}
bool spn12tet3::isinsc(const frq_t& frq_in) const {
	base_ntl_idx_t idx = base_ntl_idx(frq_in);
	return idx.is_valid;
}
bool spn12tet3::isinsc(const note_t& note_in) const {
	// Both the ntl and frq must be valid, AND the ntl => by the frq must be the same.  
	// TODO:  Still have to verify the oct => frq == oct of note_in
	base_ntl_idx_t idx_ntl = base_ntl_idx(note_in.ntl);
	if (!idx_ntl.is_valid) { return false; }
	base_ntl_idx_t idx_frq = base_ntl_idx(note_in.frq);
	if (!idx_frq.is_valid) { return false; }

	return (idx_ntl.idx_shift == idx_frq.idx_shift);
}

spn12tet3::scd3_t spn12tet3::to_scd(const ntl_t& ntl, const octn_t& o) const {
	base_ntl_idx_t idx_ntl = base_ntl_idx(ntl);
	if (!idx_ntl.is_valid) { std::abort(); }
	int scd_idx = idx_ntl.idx_shift + (o.to_int())*(m_ntls.size());
	return spn12tet3::scd3_t {scd_idx};
}
spn12tet3::scd3_t spn12tet3::to_scd(const frq_t& frq) const {
	base_ntl_idx_t idx = base_ntl_idx(frq);
	if (!idx.is_valid) { std::abort(); }
	return  spn12tet3::scd3_t {idx.idx_shift};
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
spn12tet3::base_ntl_idx_t spn12tet3::base_ntl_idx(const ntl_t& ntl) const {
	base_ntl_idx_t res {};
	auto it = std::find(m_ntls.begin(),m_ntls.end(),ntl);
	
	res.is_valid = (it==m_ntls.end());
	res.idx_noshift = static_cast<int>(it-m_ntls.begin());
	res.idx_shift = res.idx_noshift+m_shift_scd;
	return res;
}

// Private method
spn12tet3::base_ntl_idx_t spn12tet3::base_ntl_idx(const frq_t& frq_in) const {
	base_ntl_idx_t res {};
	int idx = n_eqt(frq_in, m_pstd.ref_note.frq, m_pstd.ntet, m_pstd.gen_int);
	if (!aprx_int(idx)) {
		res.is_valid = false;
		return;
	}

	res.idx_noshift = (idx+m_ntls.size())%(m_ntls.size);
	res.idx_shift = res.idx_noshift + m_shift_scd;
	res.is_valid = true;
	return res;
}


