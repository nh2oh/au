#pragma once
#include <vector>


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


int comdenom(std::vector<frac> const&);

// "Rational approximation":
frac rapprox(double,int);  // number-to-approximate, max-denominator



