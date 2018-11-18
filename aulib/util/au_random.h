#pragma once
#include <vector>
#include <array>
#include <random>
#include <numeric> // accumulate() in mean()
#include <exception>  // std::abort()

//
// Functions that make interacting with the c++ STL <random> library slightly
// less of a pain in the ass.  
//

std::mt19937 new_randeng(bool randseed=true);

std::vector<int> urandi(int,int,int);  // n, min, max; range: [min,max]
std::vector<int> urandi(int,int,int,std::mt19937&);  // n, min, max; range: [min,max]
std::vector<double> urandd(int,double,double);
std::vector<double> urandd(int,double,double,std::mt19937&);

std::vector<int> nrandi(double,double,int);  // mean, var, number-to-generate


// Normal probability _density_ evaluated at x
// Converts all the T's to double's ... good?  Force the user to
// do this?  ???
// p(x) = 1/(2*sqrt(2*pi)) * e^( -(1/2)*((x-u)/s)^2 )
template<typename T>
double normprob(T u, T s, T x) {
	double sd = static_cast<double>(s);
	double ud = static_cast<double>(u);
	double xd = static_cast<double>(x);
	double pi {3.14159265358979323846};
	double pre = (1.0/sd)*(1/std::sqrt(2.0*pi));  // "prefactor"
	double e = (-0.5)*std::pow(((xd-ud)/sd),2);  // "exponent"

	return pre*std::exp(e);
}

// Normal probability _density_ evaluated at each value of x.
template<typename T>
std::vector<double> normpdf(const std::vector<T>& x, T u, T s) {
	std::vector<double> p {};  p.reserve(x.size());
	for (int i=0; i<x.size(); ++i) {
		p.push_back(normprob(u,s,x[i]));
	}
	return p;
}


std::vector<size_t> randset(int const&, const std::vector<double>&, std::mt19937&);

template<typename T>
std::vector<T> randelems(const std::vector<T>& s, int n, 
	const std::vector<double>& p, std::mt19937& re) {
	auto ridxs = randset(n,p,re);  // "random indices"

	std::vector<T> res {}; // "random elements"
	for (int i=0; i<ridxs.size(); ++i) {
		res.push_back(s[ridxs[i]]);
	}

	return res;
}

// n random elements from s chosen w/a uniform probability dist
template<typename T>
std::vector<T> urandelems(const std::vector<T>& s, int n, std::mt19937& re) {
	auto ridxs = urandi(n,0,s.size()-1,re);  // NB:  size()-1

	std::vector<T> res(n,T{}); // "result"
	for (int i=0; i<ridxs.size(); ++i) {
		res[i] = s[ridxs[i]];
	}

	return res;
}

std::vector<double> normalize_probvec(std::vector<double>);


template <typename T>
T mean(std::vector<T> v) {
	T tot = std::accumulate(v.begin(),v.end(),T{0});
	return tot/v.size();
};
template <typename T, int N>
T mean(std::array<T,N> v) {
	T tot = std::accumulate(v.begin(),v.end(),T{0});
	return tot/v.size();
};

// Pearson's linear correlation coefficient
// a,b must be the same size
template <typename T>
T corr(const std::vector<T>& a, const std::vector<T>& b) {
	if (a.size() != b.size()) {
		std::abort();
	}
	T ma = mean(a);	T mb = mean(b);
	T sum_a_shift {0}; T sum_a_shift_sq {0};
	T sum_b_shift {0}; T sum_b_shift_sq {0};
	T sum_a_shift_b_shift {0};
	for (int i=0; i<a.size(); ++i) {
		sum_a_shift += a[i]-ma;
		sum_a_shift_sq += (a[i]-ma)*(a[i]-ma);
		sum_b_shift += b[i]-mb;
		sum_b_shift_sq += (b[i]-mb)*(b[i]-mb);
		sum_a_shift_b_shift += (a[i]-ma)*(b[i]-mb);
	}

	return sum_a_shift_b_shift/std::sqrt(sum_a_shift_sq*sum_b_shift_sq);
};

// Pearson's linear correlation coefficient
// a,m[i] must be the same size for all i (checked by corr(vector,vector>)
template <typename T>
std::vector<T> corr(const std::vector<T>& a, const std::vector<std::vector<T>>& m) {
	std::vector<T> res(m.size(),0.0);
	for (int i=0; i<m.size(); ++i) {
		res[i] = corr(a,m[i]);
	}
	return res;
};




