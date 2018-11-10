#include "spn12tet3.h"
#include "scale.h"  // n_eqt(), etc
#include "..\util\au_algs_math.h"  // aprx_int()
#include "..\util\au_util.h"  // bsprintf()
#include "..\types\frq_t.h"
#include <algorithm>
#include <exception>
#include <string>
#include <array>
#include <regex>  // for parse_ntstr()


const std::array<std::array<char,2>,12> spn12tet3::m_ntls {{'C','C#','D','D#',
		'E','F','F#','G','G#','A','A#','B'}};
//const std::array<char,12> spn12tet3::m_ntls_nomod {'C','D',
//		'E','F','G','A','B'};
//const std::array<char,2> spn12tet3::m_mods {'#','&'};


spn12tet3::spn12tet3(pitch_std ps) {
	if (ps.gen_int <= 0 || ps.ntet <= 0) { std::abort(); }
	auto ref_ntl_parsed = spn12tet3::parse_ntstr(ps.ref_ntl);
	if (!ref_ntl_parsed.is_valid) { std::abort(); }

	m_pstd = ps;

	m_shift_scd = (ref_ntl_parsed.oct)*(spn12tet3::m_ntls.size()) + ref_ntl_parsed.ntl_idx;
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
			(curr_scd-zero),(*curr_scd).print(),(*curr_scd).frq());
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

bool spn12tet3::isinsc(const std::string& s) const {
	auto ntstr_parsed = spn12tet3::parse_ntstr(s);
	return ntstr_parsed.is_valid;
}
bool spn12tet3::isinsc(const frq_t& frq_in) const {
	return aprx_int(n_eqt(frq_in, m_pstd.ref_frq, m_pstd.ntet, m_pstd.gen_int));
}

spn12tet3::scd3_t spn12tet3::to_scd(const std::string& s) const {
	auto ntstr_parsed = parse_ntstr(s);
	if (!ntstr_parsed.is_valid) {
		// This should be impossible: note is of type spn12tet3::note_t,
		// so this error implies an error in maintaining the note_t invariant.  
		std::abort();
	}
	int scd_idx = (ntstr_parsed.ntl_idx) + (ntstr_parsed.oct)*(spn12tet3::m_ntls.size());
	return spn12tet3::scd3_t {scd_idx};
}
spn12tet3::scd3_t spn12tet3::to_scd(const spn12tet3::note_t& note) const {
	auto ntstr_parsed = parse_ntstr(note.print());
	if (!ntstr_parsed.is_valid || !isinsc(note.frq())) {
		// This should be impossible: note is of type spn12tet3::note_t,
		// so this error implies an error in maintaining the note_t invariant.  
		std::abort();
	}
	int scd_idx = (ntstr_parsed.ntl_idx) + (ntstr_parsed.oct)*(spn12tet3::m_ntls.size());
	return spn12tet3::scd3_t {scd_idx};
}
std::vector<spn12tet3::scd3_t> spn12tet3::to_scd(const std::vector<spn12tet3::note_t>& ntls) const {
	std::vector<spn12tet3::scd3_t> scds {}; scds.reserve(ntls.size());
	for (const auto& e : ntls) {
		scds.push_back(to_scd(e));
	}
	return scds;
}

spn12tet3::ntstr_parsed spn12tet3::parse_ntstr(const std::string& s) {
	spn12tet3::ntstr_parsed result {};

	if (s.size() == 0) {
		result.is_valid = false;
		return result;
	}

	bool consume_first_sharp {false};
	int i=0;
	if (s.size() == 1 || (s.size()>1 && s[1]!='#')) {
		// Only match the letter
		for (; i<spn12tet3::m_ntls.size();++i) {
			if (spn12tet3::m_ntls[i][0] == s[0]) { break; }
		}
	} else if (s.size() > 1 && s[1]=='#') {
		// Start by only matching the letter
		for (; i<spn12tet3::m_ntls.size();++i) {
			if (spn12tet3::m_ntls[i][0] == s[0]) { break; }
		}
		if (m_ntls[i][0] != 'B' && m_ntls[i][0] != 'E') {
			i = (i+1)%(m_ntls.size());
			consume_first_sharp = true;
		}
	}

	if (i==m_ntls.size()) {  // Didn't find the note letter
		result.is_valid = false;
		return result;
	} else {
		result.ntl = m_ntls[i];
		result.ntl_idx = i;
	}


	std::regex rx("([#&]+)?(?:\\((-?\\d+)\\))?");
	std::smatch rx_matches;
	if (!std::regex_match(s.begin()+1,s.end(), rx_matches, rx)) {
		result.is_valid=false;
		return result;
	}
	// Sharps+flats
	if (rx_matches[0].matched) {
		result.n_sharp = std::count(s.begin(),s.end(),'#');
		result.n_flat = std::count(s.begin(),s.end(),'&');
	} else {
		result.n_sharp = 0;
		result.n_flat = 0;
	}
	if (consume_first_sharp) {
		--(result.n_sharp);
	}
	int net_sharp = result.n_sharp-result.n_flat;
	result.ntl_idx = (i+net_sharp)%(spn12tet3::m_ntls.size());

	// Octave designation
	if (rx_matches[1].matched) {
		result.oct = std::stoi(rx_matches[1].str());
		result.oct_specified = true;
	} else {
		result.oct = 0;
		result.oct_specified = false;
	}

	return result;
}











spn12tet3::note_t::note_t(const std::string& s) {
	auto ntstr_parsed = spn12tet3::parse_ntstr(s);
	if (!ntstr_parsed.is_valid) {
		std::abort();
	}
	spn12tet3::scd3_t scd {s};
	spn12tet3::note_t temp_note {*scd};
	m_frq = temp_note.m_frq;
	m_ntstr = temp_note.m_ntstr;
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


