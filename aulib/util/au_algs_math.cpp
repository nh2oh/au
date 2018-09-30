#include "au_algs_math.h"
#include <cmath>  // sqrt()
#include <numeric>  // lcm(), gcd()
#include <optional>
#include <limits>
#include <map>



// Linear regression
std::optional<linreg_result> linreg(std::vector<double> const& x, 
	std::vector<double> const& y) {
	if (x.size() != y.size() || x.size() == 0) {
		return {};
	}
	if (x.size() > std::numeric_limits<int>::max()) {
		return {};
	}
	const auto n = x.size();

	double sx = 0.0; double sx2 = 0.0;
	double sxy = 0.0;
	double sy = 0.0; double sy2 = 0.0;
	for (size_t i=0; i<n; ++i) {
		sx += x[i]; sx2 += (x[i])*(x[i]);
		sxy += (x[i])*(y[i]);
		sy += y[i]; sy2 += (y[i])*(y[i]);
	}

	const double denom = n*sx2 - sx*sx;
	if (denom <= 0) { return {}; }

	struct linreg_result res {0.0,0.0,0.0};
	res.slope = (n*sxy - sx*sy)/denom;
	res.intercept = (sy*sx2 - sx*sxy)/denom;
	res.regcoef = (sxy-sx*sy/n) / std::sqrt((sx2 - (sx*sx)/n)*(sy2 - (sy*sy)/n));

	return res;
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



