#include "au_algs_math.h"
#include <cmath>  // sqrt()
#include <numeric>  // lcm(), gcd()
#include <optional>
#include <limits>
#include <map>



// Linear regression


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



