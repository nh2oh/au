#include "spn12tet3.h"
#include "scale.h"  // n_eqt(), etc
#include "..\util\au_algs_math.h"  // aprx_int()
#include "..\util\au_util.h"  // bsprintf()
#include "..\types\frq_t.h"
#include <algorithm>
#include <exception>
#include <string>
#include <array>

const std::array<char,7> spn12tet3::ntl3_t::base_ntls {'C','D','E','F','G','A','B'};
const std::array<char,2> spn12tet3::ntl3_t::ntl_modifiers {'#','&'};

bool spn12tet3::ntl3_t::is_ntl(const std::string& s) {
	if (s.size() == 0) { return false; }

	auto it = std::find(spn12tet3::ntl3_t::base_ntls.begin(),base_ntls.end(),s[0]);
	if (it == base_ntls.end()) { return false; }

	if (s.size() > 1) {
		auto it = std::find_if(s.begin()+1,s.end(),
			[](char c) {
				return !(c==ntl_modifiers[0] || c==ntl_modifiers[1]);
			});
		if (it!=s.end()) { return false; }
	}

	return true;
}

spn12tet3::ntl3_t::ntl3_t(const std::string& s) {
	if (!is_ntl(s)) { std::abort(); }
	m_ntl = s[0];
}
spn12tet3::ntl3_t::ntl3_t(const char* c, int o) {
	if (!is_ntl(c)) { std::abort(); }
	//m_ntl = std::string {c};
	m_ntl = *c;
	m_oct = o;
}
spn12tet3::ntl3_t::ntl3_t(const std::string& s, int o) {
	if (!is_ntl(s)) { std::abort(); }
	//m_ntl = s;
	m_ntl = s[0];
	m_oct = o;
}

std::string spn12tet3::ntl3_t::print() const {
	return {m_ntl + m_mods + "(" + std::to_string(m_oct) + ")"};
}

bool spn12tet3::ntl3_t::operator==(const spn12tet3::ntl3_t& rhs) const {
	return m_ntl == rhs.m_ntl;
}
bool spn12tet3::ntl3_t::operator<(const spn12tet3::ntl3_t& rhs) const {
	return m_ntl < rhs.m_ntl;
}

bool operator!=(const spn12tet3::ntl3_t& lhs, const spn12tet3::ntl3_t& rhs) {
	return !(lhs==rhs);
}
bool operator>(const spn12tet3::ntl3_t& lhs, const spn12tet3::ntl3_t& rhs) {
	return !(lhs<rhs || lhs==rhs);
}

bool operator<=(const spn12tet3::ntl3_t& lhs, const spn12tet3::ntl3_t& rhs) {
	return (lhs<rhs || lhs==rhs);
}
bool operator>=(const spn12tet3::ntl3_t& lhs, const spn12tet3::ntl3_t& rhs) {
	return (lhs>rhs || lhs==rhs);
}

bool spn12tet3::is_ntl(const std::string& s) {
	return spn12tet3::ntl3_t::is_ntl(s);
}






spn12tet3::spn12tet3(spn12tet3::pitch_std ps) {
	if (ps.gen_int <= 0 || ps.ntet <= 0) { std::abort(); }
	m_ps = ps;

	auto it = std::find(spn12tet3::ntl3_t::base_ntls.begin(),
		spn12tet3::ntl3_t::base_ntls.end(), m_ps.ref_ntl.m_ntl);
	if (it == spn12tet3::ntl3_t::base_ntls.end()) {
		// This should be impossible: pitch_std.ref_ntl is of type spn12tet3::ntl3_t,
		// so this error implies an error in the ntl3_t ctor.  
		std::abort();
	}
	m_shift_scd = (ps.ref_oct)*(spn12tet3::N) + 
		static_cast<int>(it-spn12tet3::ntl3_t::base_ntls.begin());
	// Expect 57 for a ref pitch of A(4)
}


std::string spn12tet3::name() const {
	return m_name;
}
std::string spn12tet3::description() const {
	return m_description;
}
bool spn12tet3::isinsc(const std::string& s) const {
	return spn12tet3::ntl3_t::is_ntl(s);
}
bool spn12tet3::isinsc(frq_t frq_in) const {
	return aprx_int(n_eqt(frq_in, m_ps.ref_frq, m_ps.ntet, m_ps.gen_int));
}
std::string spn12tet3::print() const {
	return spn12tet3::print(scd_t {0}, scd_t {N});
}
std::string spn12tet3::print(scd_t from, scd_t to) const {
	std::string s {};
	s = dbk::bsprintf("%s\n%s\n\n",m_name,m_description);

	for (scd_t curr_scd=from; curr_scd<to; ++curr_scd) {
		//std::string curr_line {};
		s += dbk::bsprintf("%d:\t%s\t%.3f\n",
			curr_scd.to_int(),to_ntl(curr_scd).print(),to_frq(curr_scd)/frq_t{1});
	}
	s += "\n\n";
	return s;
}

spn12tet3::ntl_t spn12tet3::to_ntl(scd_t scd) const {  // Reads m_default_valid_ntls directly
	return spn12tet3::ntl3_t {&spn12tet3::ntl3_t::base_ntls[scd.rscd().to_int()],scd.octn()};
	// TODO:  Look at how i'm passing a ptr here w/ operator& ...
}
std::vector<spn12tet3::ntl_t> spn12tet3::to_ntl(const std::vector<scd_t>& scds) const {
	std::vector<spn12tet3::ntl_t> ntls {}; ntls.reserve(scds.size());
	for (const auto& e : scds) {
		ntls.push_back(to_ntl(e));
	}
	return ntls;
}
spn12tet3::ntl_t spn12tet3::to_ntl(frq_t frq) const {  // wrapper
	scd_t scd = to_scd(frq);
	return to_ntl(scd);
}

frq_t spn12tet3::to_frq(scd_t scd) const {  // Calls frq_eqt() directly
	return frq_eqt(scd.to_int()-m_shift_scd,m_ps.ref_frq,m_ps.ntet,m_ps.gen_int);
}
std::vector<frq_t> spn12tet3::to_frq(const std::vector<scd_t>& scds) const {
	std::vector<frq_t> frqs {}; frqs.reserve(scds.size());
	for (const auto& e : scds) {
		frqs.push_back(to_frq(e));
	}
	return frqs;
}
frq_t spn12tet3::to_frq(spn12tet3::ntl_t ntl) const {  // wrapper
	scd_t scd = to_scd(ntl);
	return to_frq(scd);
}
std::vector<frq_t> spn12tet3::to_frq(const std::vector<spn12tet3::ntl3_t>& ntls) const {
	std::vector<frq_t> frqs {}; frqs.reserve(ntls.size());
	for (const auto& e : ntls) {
		frqs.push_back(to_frq(e));
	}
	return frqs;
}
spn12tet3::scd_t spn12tet3::to_scd(frq_t frq) const {  //  Calls n_eqt() directly
	double dn_approx = n_eqt(frq,m_ps.ref_frq,m_ps.ntet,m_ps.gen_int);
	if (!aprx_int(dn_approx)) { std::abort(); }
	return scd_t {static_cast<int>(std::round(dn_approx+m_shift_scd))};
}
spn12tet3::scd_t spn12tet3::to_scd(spn12tet3::ntl_t ntl) const {  // Reads m_default_valid_ntls directly
	auto it = std::find(spn12tet3::ntl_t::base_ntls.begin(),
		spn12tet3::ntl_t::base_ntls.end(),ntl.m_ntl);
	if (it == spn12tet3::ntl3_t::base_ntls.end()) {
		// This should be impossible: ntl is of type spn12tet3::ntl3_t,
		// so this error implies an error in maintaining the ntl3_t invariant.  
		std::abort();
	}
	int ntl_idx = it-spn12tet3::ntl3_t::base_ntls.begin();
	return scd_t {ntl_idx, ntl.m_oct};
}
std::vector<spn12tet3::scd_t> spn12tet3::to_scd(const std::vector<spn12tet3::ntl_t>& ntls) const {
	std::vector<spn12tet3::scd_t> scds {}; scds.reserve(ntls.size());
	for (const auto& e : ntls) {
		scds.push_back(to_scd(e));
	}
	return scds;
}

