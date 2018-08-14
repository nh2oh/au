#include "au_frac.h"
#include "au_algs_math.h"  // isapproxeq() in rapprox()
#include <numeric>  // lcm(), gcd()
#include <vector> // comdenom


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




int comdenom(std::vector<frac> const& fracs) {
	int d = 1;
	for (auto e : fracs) {  // copy:  reduce() alters e
		d = std::lcm((e.reduce()).denom,d);
	}
	return d;
}


// Rational approximation
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



