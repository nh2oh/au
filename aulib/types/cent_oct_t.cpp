#include "cent_oct_t.h"
#include "frq_t.h"
#include "..\util\au_error.h"
#include "..\util\au_algs_math.h"
#include "..\util\au_util.h"  // int_suffix()
#include <map>
#include <vector>
#include <string>
#include <cmath>  // pow(), log2()
#include <cstdlib> // div()... not in <cmath>


//-----------------------------------------------------------------------------
// The cent_t class
const std::map<cent_t,std::vector<std::string>> cent_t::m_cipmap_cent {
	{0_cent,{"u"}}, {100_cent,{"m2"}}, {200_cent,{"M2"}},
	{300_cent,{"m3"}}, {400_cent,{"M3"}},
	{500_cent,{"P4"}}, {600_cent,{"A4","d5"}}, {700_cent,{"P5"}},
	{800_cent,{"m6"}}, {900_cent,{"M6"}},
	{1000_cent,{"m7"}}, {1100_cent,{"M7"}},
	{1200_cent,{"P8","O"}}
};


cent_t::cent_t(frq_t frq_from, frq_t frq_to) {
	m_cents = 1200*std::log2(frq_to/frq_from);
}

cent_t::cent_t(double cents_in) { 
	m_cents = cents_in;
}

cent_t::cent_t(oct_t oct_in) { 
	m_cents = 1200*oct_in.to_double();
}

double cent_t::to_double() const {
	return m_cents;
}

 // Return the abbreviated common name idx indicated by prefer_name
std::string cent_t::to_acname(int name_idx) const {
	std::string s {};
	const auto nhundreds = std::div(static_cast<int>(m_cents),100);
	if (nhundreds.rem != 0) { return s; }
	
	if (nhundreds.quot <= 12 && nhundreds.quot >= -12) {
		auto map_result = m_cipmap_cent.find(cent_t{std::abs(m_cents)});
		if (map_result == m_cipmap_cent.end()) { return s;	}

		auto cent_names = map_result->second;
		if (m_cents < 0) { s += "-"; }
		if (name_idx >= 0 && name_idx < cent_names.size()) {
			s += cent_names[name_idx];
		}
	} else { // abs(m_cents) > 12: 13 cents => 9, 14 cents => 10, ...
		auto common_name_number = static_cast<int>(std::round(nhundreds.quot-4));
		s += std::to_string(common_name_number) + "'" + int_suffix(common_name_number);
	}

	return s;
}


std::string cent_t::to_fcname(int prefer_name) const {
	std::string s {};
	auto acname {to_acname(prefer_name)};  // abbreviated common name
	if (acname.size()==0) { return s; }

	std::map<std::string,std::string> cip_qual_map {{"m","Minor"},{"M","Major"},{"P","Perfect"},
	{"A","Augmented"},{"d","Diminished"},{"u","Unison"},{"O","Octave"}};

	if (m_cents < 0) {
		s += "Descending ";
	} else if (m_cents > 0) {
		s += "Ascending ";
	}

	std::string rx {"(?:-)?([MmAdPuO]{1,1})?(\\d+)?(?:'[thrdndst]{2,2})?"};
	auto rx_caps = rx_match_captures(rx, acname);
	au_assert(rx_caps.has_value(), "!rx_caps");

	if (rx_caps && (*rx_caps)[1]) {
		s += cip_qual_map[*((*rx_caps)[1])] + " ";
	}

	if (rx_caps && (*rx_caps)[2]) { 
		int int_num = std::stoi(*((*rx_caps)[2]));
		s += *((*rx_caps)[2]) + "'" + int_suffix(int_num);
	}

	return s;
}

std::string cent_t::print(int prefer_name) const {
	std::string s {};
	for (int i=0; true; ++i) {
		auto curr_name = to_acname(i);
		if (curr_name.size()==0) { break; }

		s += curr_name;
		if (prefer_name == i) { s+="*"; }
		s += ", ";
	}
	s.erase(s.end()-2,s.end());
	s += " => " + std::to_string(m_cents) + " cents";

	return s;
}


bool operator==(cent_t const& lhs, cent_t const& rhs) {
	return aprx_eq(lhs.to_double(),rhs.to_double()); //(isapproxeq(lhs.to_double(),rhs.to_double(),6));
}
bool operator<(cent_t const& lhs, cent_t const& rhs) {
	return aprx_lt(lhs.to_double(),rhs.to_double()); //(lhs.to_double() < rhs.to_double());
}
bool operator>(cent_t const& lhs, cent_t const& rhs) {
	return aprx_gt(lhs.to_double(),rhs.to_double()); //(lhs.to_double() > rhs.to_double());
}
bool operator>=(cent_t const& lhs, cent_t const& rhs) {
	return lhs.to_double() > rhs.to_double() || lhs.to_double() == rhs.to_double();
	//(lhs.to_double() >= rhs.to_double());
}
bool operator<=(cent_t const& lhs, cent_t const& rhs) {
	return lhs.to_double() < rhs.to_double() || lhs.to_double() == rhs.to_double();
	//return (lhs.to_double() <= rhs.to_double());
}
cent_t operator+(cent_t const& lhs, cent_t const& rhs) {
	return cent_t {lhs.to_double() + rhs.to_double()};
}
cent_t operator-(cent_t const& lhs, cent_t const& rhs) {
	return cent_t {lhs.to_double() - rhs.to_double()};
}
cent_t operator-(cent_t const& lhs) {
	return cent_t {(-1)*lhs.to_double()};
}
cent_t operator*(cent_t const& lhs, double const& rhs) {
	return cent_t {lhs.to_double()*rhs};
}
cent_t operator*(double const& lhs, cent_t const& rhs) {
	return cent_t {lhs*rhs.to_double()};
}
double operator/(cent_t const& lhs, cent_t const& rhs) {
	return {lhs.to_double()/rhs.to_double()};
}
cent_t operator/(cent_t const& lhs, double const& rhs) {
	return cent_t {lhs.to_double()/rhs};
}
double operator/(double const& lhs, cent_t const& rhs) {
	return lhs/(rhs.to_double());
}

cent_t operator""_cent(unsigned long long literal_in) {
	return cent_t {static_cast<double>(literal_in)};
}

//-----------------------------------------------------------------------------
// The oct_t class

oct_t::oct_t(double num_octs) {
	m_oct = num_octs;
}

oct_t::oct_t(cent_t cents_in) {
	m_oct = (cents_in.to_double())/1200;
}
oct_t::oct_t(frq_t frq_num, frq_t frq_denom) {
	auto num_cents = cent_t {frq_num,frq_denom};
	m_oct = (num_cents.to_double())/1200;
}

double oct_t::to_double() const {
	return m_oct;
}

