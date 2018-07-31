#include <cmath>
#include <numeric>
#include <random>
#include <chrono>
#include "au_algs_math.h"


// Random double
std::vector<double> urandd(int n, double min, double max) {
	std::default_random_engine randeng {};
	auto t = std::chrono::system_clock::now().time_since_epoch().count();
	randeng.seed(t);
	std::uniform_real_distribution<double> rn {min,max};
	std::vector<double> rv(n,0.0);
	for (auto i=0; i<n; ++i) {
		rv[i] = rn(randeng);
	}
	return rv;
}

// Random int
std::vector<int> urandi(int n, int min, int max) {
	std::default_random_engine randeng {};
	randeng.seed(std::chrono::system_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<int> rn {min,max};
	std::vector<int> rv(n,0);
	for (auto i=0; i<n; ++i) {
		rv[i] = rn(randeng);
	}
	return rv;
}

// is approximately integer
// true if d is approximately an integer value
bool isapproxint(double d, int prec) {
	double scf = std::pow(10,prec); // "scale factor"
	double delta = std::round(d)*scf - d*scf;
	return (std::abs(delta) < 1);
}

// is approximately equal
bool isapproxeq(double lhs, double rhs, int prec) {
	double diff = std::abs(lhs-rhs);
	return (diff < (1/std::pow(10,prec)));
}

// true if a/b == integer
bool ismultiple(double a, double b, int prec) {
	return (std::abs(a/b - std::round(a/b)) < 1/std::pow(10,6));
}

int lcm(std::vector<int> const& v) {
	int result = 1;
	for (auto e:v) {
		result = std::lcm(e,result);
	}
	return result;
}

int gcd(std::vector<int> const& v) {
	int result = 1;
	for (auto e:v) {
		result = std::gcd(e,result);
	}
	return result;
}

int comdenom(std::vector<frac> const& fracs) {
	int d = 1;
	for (auto e : fracs) {  // copy:  reduce() alters e
		d = std::lcm((e.reduce()).denom,d);
	}
	return d;
}

frac rapprox(double x, int N) {  // number-to-approximate, max-denominator
	int prec = 6;

	double integral {0};
	int sign = 1;
	if (x < 0) {
		x = (-1)*x;
		sign = -1;
	}
	if (x > 1.0) {
		integral = std::floor(x);
		x = x-integral;
	}
	
	double a {0}; double b {1}; double c {1}; double d {1};
	while (b <= N && d <=N) {
		auto mediant = (a+c)/(b+d);
		if (isapproxeq(x,mediant,prec)) {
			if (b+d <= N) { 
				return sign*frac{static_cast<int>((a+c)+integral*(b+d)),static_cast<int>(b+d)};
			} else if (d > b) {
				return sign*frac{static_cast<int>(c+integral*d),static_cast<int>(d)};
			} else {
				return sign*frac{static_cast<int>(a+integral*b),static_cast<int>(b)};
			}
		} else if (x > mediant) {
			a = a+c; b = b+d;
		} else {
			c = a+c; d = b+d;
		}
	}

	if (b > N) {
		return sign*frac{static_cast<int>(c+integral*d),static_cast<int>(d)};
	} else {
		return sign*frac{static_cast<int>(a+integral*b),static_cast<int>(b)};
	}
}

//-----------------------------------------------------------------------------
// Class frac

frac& frac::reduce() {
	int cd = std::gcd(num,denom);
	num = num/cd;
	denom = denom/cd;
	return *this;
}
double frac::to_double() const {
	return (static_cast<double>(num)/static_cast<double>(denom));
}

bool operator==(frac const& lhs, frac const& rhs) {
	frac diff {0,1};
	diff = lhs-rhs;
	return (diff.num == 0);
}
bool operator>(frac const& lhs, frac const& rhs) {
	return (lhs.to_double() > rhs.to_double());
}
bool operator<(frac const& lhs, frac const& rhs) {
	return (lhs.to_double() < rhs.to_double());
}

frac operator+(frac const& lhs, frac const& rhs) {
	int cd = std::lcm(lhs.denom, rhs.denom);
	frac result {0,0};
	result.num = (cd/lhs.denom)*(lhs.num) + (cd/rhs.denom)*(rhs.num);
	result.denom = cd;
	return result;
}
frac operator+(frac const& lhs, int const& rhs) {  // (a/b) + c
	return frac {lhs.num+rhs*lhs.denom, lhs.denom};
}
frac operator+(int const& lhs, frac const& rhs) {  // c + (a/b)
	return (rhs+lhs);
}

frac operator-(frac const& lhs, frac const& rhs) {
	int cd = std::lcm(lhs.denom, rhs.denom);
	frac result {0,0};
	result.num = (cd/lhs.denom)*(lhs.num) - (cd/rhs.denom)*(rhs.num);
	result.denom = cd;
	return result;
}
frac operator-(frac const& lhs, int const& rhs) {  // (a/b) - c
	return frac {lhs.num-rhs*lhs.denom, lhs.denom};
}
frac operator-(int const& lhs, frac const& rhs) {  // c - (a/b)
	return (rhs-lhs);
}

frac operator*(int const& fctr_in, frac const& frac_in) {
	return frac {frac_in.num*fctr_in, frac_in.denom};
}
frac operator*(frac const& frac_in, int const& fctr_in) {
	return fctr_in*frac_in;
}
frac operator*(frac const& lhs, frac const& rhs) {  // (a/b)*(c/d)
	return frac {(lhs.num)*(rhs.num), (lhs.denom)*(rhs.denom)};
}

frac operator/(int const& fctr_in, frac const& frac_in) {
	return frac {frac_in.denom*fctr_in, frac_in.num};
}
frac operator/(frac const& frac_in, int const& fctr_in) {
	return frac {frac_in.num, fctr_in*(frac_in.denom)};
}
frac operator/(frac const& lhs, frac const& rhs) {  // (a/b)/(c/d)
	return frac {(lhs.num)*(rhs.denom), (lhs.denom)*(rhs.num)};
}






