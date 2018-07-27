#pragma once
#include <vector>
#include <cstdlib> // std::abs for template roundstep(), std::round for template roundquant()

std::vector<double> urandd(int,double,double);
std::vector<int> urandi(int,int,int);

bool isapproxint(double,int); // is approximately integer
bool isapproxeq(double, double, int);
bool ismultiple(double, double, int); // true if arg1/arg2 == integer

int lcm(std::vector<int> const&);
int gcd(std::vector<int> const&);

class frac;
int comdenom(std::vector<frac> const&);

// "Rational approximation":
frac rapprox(double,int);  // number-to-approximate, max-denominator


// subject-to-round, allowed-round-targets
// allowed_round_targets _must_ be sorted smallest -> largest
template<typename T> T roundset(T const&, std::vector<T> const&);

// subject-to-round, allowed-round-targets
// allowed_round_targets _must_ be sorted smallest -> largest
template<typename T>
T roundset(T const& subject, std::vector<T> const& targets) {
	for (auto i=1; i<targets.size(); ++i) {
		auto s_t = (subject-targets[i]);
		if (s_t < (subject-subject)) {
			s_t = (targets[i]-subject);
		}
		auto s_tp = (subject-targets[i-1]);
		if (s_tp < (subject-subject)) {
			s_tp = targets[i-1]-subject;
		}

		if (s_t > s_tp) {
			return targets[i-1];
		}
	}
	return targets.back();
};


// vector of values to fuzz, random-fraction-of-value to add to each element
template<typename T> std::vector<T> fuzzset(std::vector<T> const&, double const&);

// vector of values to fuzz, random-fraction-of-value to add to each element
template<typename T>
std::vector<T> fuzzset(std::vector<T> const& v, double const& fuzz_frac) {
	std::vector<double> rns = urandd(v.size()+1,-fuzz_frac,fuzz_frac);
	auto v_fuzzed = v;
	for (int i=0; i<v.size(); ++i) {
		v_fuzzed[i] = v_fuzzed[i] + (v_fuzzed[i])*(rns[i]);
	}
	return v_fuzzed;
};


// subject-to-round, step
template<typename T> T roundquant(T const&, T const&);

// subject-to-round, step
template<typename T>
T roundquant(T const& subject, T const& step) {
	auto n = std::round((subject/step));
	return std::abs(n)*step;
};

// difference of adjacent values
template<typename T> std::vector<T> diffadj(std::vector<T> const&);

// difference of adjacent values
template<typename T>
std::vector<T> diffadj(std::vector<T> const& v) {
	std::vector<T> dv {};
	for (auto i=1; i<v.size(); ++i) {
		dv.push_back(v[i]-v[i-1]);
	}
	return dv;
};

//-----------------------------------------------------------------------------
// Crappy homebrew fraction class
class frac {
public:
	int num {0};
	int denom {1};
	double to_double() const;
	frac& reduce();
};
bool operator==(frac const&, frac const&);
bool operator>(frac const&, frac const&);
bool operator<(frac const&, frac const&);
frac operator+(frac const&, frac const&);  // (a/b) + (c/d)
frac operator+(frac const&, int const&);  // (a/b) + c
frac operator+(int const&, frac const&);  // c + (a/b)
frac operator-(frac const&, frac const&);  // (a/b) - (c/d)
frac operator-(frac const&, int const&);  // (a/b) - c
frac operator-(int const&, frac const&);  // c - (a/b)
frac operator*(int const&, frac const&);  // c*(a/b)
frac operator*(frac const&, int const&);  // (a/b)*c
frac operator*(frac const&, frac const&);  // (a/b)*(c/d)
frac operator/(int const&, frac const&);  // c/(a/b)
frac operator/(frac const&, int const&);  // (a/b)/c
frac operator/(frac const&, frac const&);  // (a/b)/(c/d)

