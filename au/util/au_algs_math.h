#pragma once
#include <vector>

class frac;


bool isapproxint(double,int); // is approximately integer
bool isapproxeq(double, double, int);
bool ismultiple(double, double, int); // true if arg1/arg2 == integer

int lcm(std::vector<int> const&);
int gcd(std::vector<int> const&);

int comdenom(std::vector<frac> const&);

// "Rational approximation":
frac rapprox(double,int);  // number-to-approximate, max-denominator


// subject-to-round, allowed-round-targets
// allowed_round_targets _must_ be sorted smallest -> largest
template<typename T> T roundquant(T const&, std::vector<T> const&);

// subject-to-round, allowed-round-targets
// allowed_round_targets _must_ be sorted smallest -> largest
template<typename T>
T roundquant(T const& subject, std::vector<T> const& targets) {
	for (auto i=1; i<targets.size(); ++i) {
		if (std::abs(subject-targets[i]) > std::abs(subject - targets[i-1])) {
			return targets[i-1];
		}
	}
	return targets.back();
}

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

