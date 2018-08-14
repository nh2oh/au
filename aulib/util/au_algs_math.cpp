#include "au_algs_math.h"
#include <cmath>  // sqrt()
#include <numeric>  // lcm(), gcd()
#include <optional>
#include <limits>

// Linear regression
std::optional<linreg_result> linreg(std::vector<double> const& x, 
	std::vector<double> const& y) {
	if (x.size() != y.size() || x.size() == 0) {
		return {};
	}
	if (x.size() > std::numeric_limits<int>::max()) {
		return {};
	}
	int n {static_cast<int>(x.size())};

	double sx = 0.0; double sx2 = 0.0;
	double sxy = 0.0;
	double sy = 0.0; double sy2 = 0.0;
	for (int i=0; i<n; ++i) {
		sx += x[i]; sx2 += (x[i])*(x[i]);
		sxy += (x[i])*(y[i]);
		sy += y[i]; sy2 += (y[i])*(y[i]);
	}

	double denom = n*sx2 - sx*sx;
	if (denom <= 0) { return {}; }

	struct linreg_result res {0.0,0.0,0.0};

	res.slope = (n*sxy - sx*sy)/denom;
	res.intercept = (sy*sx2 - sx*sxy)/denom;
	res.regcoef = (sxy-sx*sy/n) / std::sqrt((sx2 - (sx*sx)/n)*(sy2 - (sy*sy)/n));

	return res; 
}

// "is d approximately [an] integer?"
// true if d is within 10^-prec of an integer value
bool isapproxint(double d, int prec) {
	return isapproxeq(d,std::round(d),prec);
}

// "is lhs approximately equal to rhs?"
// true if lhs and rhs are within 10^-prec of eachother
bool isapproxeq(double lhs, double rhs, int prec) {
	return (std::abs(lhs-rhs) < std::pow(10,-prec));
}

// "is a an integer multiple of b?"
// true if a/b is within 10^-prec of an integer.  
bool ismultiple(double a, double b, int prec) {
	return isapproxeq(a/b,std::round(a/b),prec);
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



