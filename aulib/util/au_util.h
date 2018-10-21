#pragma once
#include "tinyformat2.h"
#include <string>
#include <optional>
#include <vector>
#include <regex>
#include <array>

// Just a wrapper to tfm2
template<typename... Args>
std::string bsprintf(const char* fmt, const Args&... args) {
	std::ostringstream oss;
	tfm::format(oss, fmt, args...);
	return oss.str();
}

std::string int_suffix(int const&);

// Specify regex as string (arg 1); function calls the std::regex
// constructor
std::optional<std::vector<std::optional<std::string>>> 
	rx_match_captures(std::string const&, std::string const&);

// Call the std::regex constructor yourself and pass the object in
std::optional<std::vector<std::optional<std::string>>> 
	rx_match_captures(std::regex const&, std::string const&);


// Waits a v. short amount of time... sometimes useful for debugging
int wait();


// f_e => fmt str for each element
// ex:  f+e = "%f5.3\n"
template<typename T>
std::string printv(const std::vector<T>& v, const char *f_e) {
	std::string s {};
	for (const auto& e : v) {
		s += bsprintf(f_e,e);
	}

	return s;
};

// m[c][r];  m[i] => 1xn vector representing col i; m[i].size() == nrows
// and is the same for all i.  
// f_e => fmt str for each element & includes the col seperator
template<typename T, std::size_t NC, std::size_t NR>
std::string printm(const std::array<std::array<T,NR>,NC>& v,
	const char *f_e, const char *rsep="\n") {
	std::string s {};
	size_t nrows {v[0].size()};
	size_t ncols {v.size()};
	for (size_t r=0; r<nrows; ++r) {
		for (size_t c=0; c<ncols; ++c) {
			s += bsprintf(f_e,v[c][r]);
		}  // To next c in r
		if (r<(nrows-1)) {  // Not the last iter
			s += bsprintf(rsep);
		}
	} // To next r

	return s;
};


// m[c][r];  m[i] => 1xn vector representing col i; m[i].size() == nrows
// and is the same for all i.  
// f_e => fmt str for each element & includes the col seperator
template<typename T>
std::string printm(const std::vector<std::vector<T>>& v,
	const char *f_e, const char *rsep="\n") {
	std::string s {};
	size_t nrows {v[0].size()};
	size_t ncols {v.size()};
	for (size_t r=0; r<nrows; ++r) {
		for (size_t c=0; c<ncols; ++c) {
			s += bsprintf(f_e,v[c][r]);
		}  // To next c in r
		if (r<(nrows-1)) {  // Not the last iter
			s += bsprintf(rsep);
		}
	} // To next r

	return s;
};

