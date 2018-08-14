#include "frq_t.h"
#include "ntl_t.h"
#include "..\util\au_error.h"
#include "..\util\au_algs_math.h"
#include "..\util\au_util.h"  // int_suffix()
#include <map>
#include <vector>
#include <string>
#include <cmath>  // pow(), log2()
#include <cstdlib> // div()... not in <cmath>

//-----------------------------------------------------------------------------
// The frq_t class

frq_t::frq_t(double frq_in) {
	au_assert(frq_in > 0);
	m_frq = frq_in;
}

std::string frq_t::print() const {
	return std::to_string(m_frq);
}

double frq_t::to_double() const {
	return m_frq;
}

bool operator==(frq_t const& lhs, frq_t const& rhs) {
	//double scf = std::pow(10,6); // "scale factor" ...note the hardcoded precision 6
	//double delta = std::abs(lhs.to_double() - rhs.to_double());
	//return(static_cast<int>(scf*delta) == 0);
	return(isapproxeq(lhs.to_double(),rhs.to_double(),6));
}
bool operator<(frq_t const& lhs, frq_t const& rhs) {
	return (lhs.to_double() < rhs.to_double());
}
bool operator>(frq_t const& lhs, frq_t const& rhs) {
	return (lhs.to_double() > rhs.to_double());
}
bool operator>=(frq_t const& lhs, frq_t const& rhs) {
	return (lhs > rhs || lhs == rhs);
}
bool operator<=(frq_t const& lhs, frq_t const& rhs) {
	return (lhs < rhs || lhs == rhs);
}

frq_t operator+(frq_t const& lhs, frq_t const& rhs) {
	return frq_t {lhs.to_double()+rhs.to_double()};
}
// frq_t constructor checks for <= 0
frq_t operator-(frq_t const& lhs, frq_t const& rhs) {
	return frq_t {lhs.to_double()-rhs.to_double()}; // Constructor ensures > 0
}
// Result is a frq_t^2, so dropping the units
double operator*(frq_t const& lhs, frq_t const& rhs) {  
	return (lhs.to_double()*rhs.to_double());
}
frq_t operator*(double const& lhs, frq_t const& rhs) {
	return frq_t {lhs*rhs.to_double()}; // Constructor ensures > 0
}
frq_t operator*(frq_t const& lhs, double const& rhs) {
	return frq_t {rhs*lhs.to_double()}; // Constructor ensures > 0
}
// Division yields a dimensionless parameter
double operator/(frq_t const& lhs, frq_t const& rhs) {
	return (lhs.to_double()/rhs.to_double());
}
// units are time, no longer frq_t
double operator/(double const& lhs, frq_t const& rhs) {  
	return lhs/(rhs.to_double()); 
}

frq_t operator/(frq_t const& lhs, double const& rhs) {
	return frq_t {(lhs.to_double())/rhs}; // Constructor ensures > 0
}

std::ostream& operator<<(std::ostream& os, frq_t const& frq_in) {
	return (os << frq_in.print());
}

frq_t operator+(frq_t const& lhs, cent_t const& rhs) {
	auto frq = (lhs.to_double())*std::pow(2,(rhs.to_double()/1200));
	return frq_t {frq};
}
frq_t operator-(frq_t const& lhs, cent_t const& rhs) {
	auto frq = (lhs.to_double())*std::pow(2,((-rhs).to_double()/1200));
	return frq_t {frq};
}

// Nonmember helper functions
frq_t plus_cent(frq_t const& frq_in, cent_t const& cent_in) {
	auto frq = frq_in*std::pow(2,(cent_in.to_double()/1200));
	return frq_t {frq};
}
frq_t plus_oct(frq_t const& frq_in, oct_t const& oct_in) {
	//auto frq = frq_in*std::pow(2,(oct_in.to_int()));
	auto frq = frq_in + oct_in;
	return frq_t {frq};
}

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

/*
const std::vector<std::vector<std::string>> cip_t::m_cipmap_name {
	,,,,,,,,,
{"9'th"},{"10'th"},{"11'th"},{"12'th"},
{"13'th"},{"14'th"},{"15'th"},{"16'th"},{"17'th"},{"18'th"},{"19'th"},
{"20'th"},{"21'st"},{"22'nd"},{"23'rd"},{"24'th"},{"25'th"},{"26'th"},
{"27'th"},{"28'th"},{"29'th"},{"30'th"},{"31'st"},{"32'nd"},{"33'rd"},
{"34'th"},{"35'th"},{"36'th"},{"37'th"},{"38'th"},{"39'th"},{"-m2"},
{"-M2"},{"-m3"},{"-M3"},{"-P4"},{"-A4","-d5"},{"-P5"},{"-m6"},{"-M6"},
{"-m7"},{"-M7"},{"-P8","-O"},{"-9'th"},{"-10'th"},{"-11'th"},{"-12'th"},
{"-13'th"},{"-14'th"},{"-15'th"},{"-16'th"},{"-17'th"},{"-18'th"},
{"-19'th"},{"-20'th"},{"-21'st"},{"-22'nd"},{"-23'rd"},{"-24'th"},
{"-25'th"},{"-26'th"},{"-27'th"},{"-28'th"},{"-29'th"},{"-30'th"},
{"-31'st"},{"-32'nd"},{"-33'rd"},{"-34'th"},{"-35'th"},{"-36'th"},
{"-37'th"},{"-38'th"},{"-39'th"}
};*/


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
	return static_cast<double>(m_cents);
}

 // Return the abbreviated common name idx indicated by prefer_name
std::optional<std::string> cent_t::to_acname(int name_idx) const {
	if (name_idx < 0) {
		return {};
	}
	std::string s {};
	auto nhundreds = std::div(static_cast<int>(m_cents),100);
	if (nhundreds.rem != 0) {
		return {};
	}
	
	if (nhundreds.quot <= 12 && nhundreds.quot >= -12) {
		auto map_result = m_cipmap_cent.find(cent_t{std::abs(m_cents)});
		if (map_result == m_cipmap_cent.end()) {
			au_error("Should never happen if between -12 and 12...");
			return {};
		}

		auto cent_names = map_result->second;
		if (name_idx >= cent_names.size()) {
			return {};
		}
  
		if (m_cents < 0) { s += "-"; }
		s += cent_names[name_idx];

	} else { // abs(m_cents) > 12: 13 cents => 9, 14 cents => 10, ...
		if (name_idx != 0) {
			return {};
		}
		auto common_name_number = static_cast<int>(std::round(nhundreds.quot-4));
		s += std::to_string(common_name_number) + "'" + int_suffix(common_name_number);
	}

	return s;
}


std::optional<std::string> cent_t::to_fcname(int prefer_name) const {
	auto acname {to_acname(prefer_name)};  // abbreviated common name
	if (!acname) { return {}; }

	std::map<std::string,std::string> cip_qual_map {{"m","Minor"},{"M","Major"},{"P","Perfect"},
	{"A","Augmented"},{"d","Diminished"},{"u","Unison"},{"O","Octave"}};

	std::string name_substr_updown {};
	std::string name_substr_qual {};
	std::string name_substr_number {};

	if (static_cast<int>(std::round(m_cents)) == 0) {
		name_substr_updown = "";
	} else if (m_cents < 0) {
		name_substr_updown = "Descending ";
	} else if (m_cents > 0) {
		name_substr_updown = "Ascending ";
	}

	std::string rx {"(?:-)?([MmAdPuO]{1,1})?(\\d+)?(?:'[thrdndst]{2,2})?"};
	auto rx_caps = rx_match_captures(rx, *acname);
	if (rx_caps) {
		if ((*rx_caps)[1]) {
			name_substr_qual = cip_qual_map[*((*rx_caps)[1])] + " ";
		}

		if ((*rx_caps)[2]) { 
			int int_num = std::stoi(*((*rx_caps)[2]));
			name_substr_number = *((*rx_caps)[2]) + "'" + int_suffix(int_num);
		}
	} else {
		au_error("!rx_caps");
	}

	return std::string {name_substr_updown + name_substr_qual + name_substr_number};
}

std::string cent_t::print(int prefer_name) const {

	std::vector<std::string> acnames {};
	int i = 0;
	while (true) {
		auto curr_name = to_acname(i);
		if (!curr_name) { break; }
		acnames.push_back(*curr_name);
		++i;
	}

	std::string s {};
	for (int j=0; j<acnames.size(); ++j) {
		s += acnames[j];

		if (acnames.size() > 1 && prefer_name == j) {
			s += "*";
		}

		if (j < (acnames.size()-1)) {
			s += ", "; // not yet the last iteration
		}
	}
	s += " => " + std::to_string(m_cents) + " cents";

	return s;
}


bool operator==(cent_t const& lhs, cent_t const& rhs) {
	return(isapproxeq(lhs.to_double(),rhs.to_double(),6));
}
bool operator<(cent_t const& lhs, cent_t const& rhs) {
	return (lhs.to_double() < rhs.to_double());
}
bool operator>(cent_t const& lhs, cent_t const& rhs) {
	return (lhs.to_double() > rhs.to_double());
}
bool operator>=(cent_t const& lhs, cent_t const& rhs) {
	return (lhs.to_double() >= rhs.to_double());
}
bool operator<=(cent_t const& lhs, cent_t const& rhs) {
	return (lhs.to_double() <= rhs.to_double());
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

