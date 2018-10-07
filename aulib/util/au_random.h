#pragma once
#include <vector>
#include <random>

//
// Functions that make interacting with the c++ STL <random> library slightly
// less of a pain in the ass.  
//

std::mt19937 new_randeng(bool randseed=true);

std::vector<int> urandi(int,int,int);
std::vector<double> urandd(int,double,double);
std::vector<double> urandd(int,double,double,std::mt19937&);

std::vector<int> nrandi(double,double,int);  // mean, var, number-to-generate

/*template<typename T>
std::vector<T> nrandrange(T min, T max, T inc, T u, T s, int n) {
	double nincs_range = (max-min)/inc;
	double nincs_u = u/inc;
	double nincs_s = s/inc;
	// Compute the dimensionless normal density function for u=1,s=1.   
	// Somehow map into T
	std::vector<double> x(std::floor(nincs_range),0);
	for (int i=0; i<x.size(); ++i) {
		x[i] = i*;
	}
}*/

// Normal probability _density_
template<typename T>
double normprob(T u, T s, T x) {
	double pi {3.14159265358979323846};
	double pre = (1.0/s)*(1/std::sqrt(2.0*pi));  // "prefactor"
	double e = (-0.5)*std::pow(((x-u)/s),2);  // "exponent"

	return pre*std::exp(e);
}

// Normal probability _density_
template<typename T>
std::vector<double> normpdf(const std::vector<T>& x, T u, T s) {
	std::vector<double> p(x.size(),0.0);
	for (int i=0; i<x.size(); ++i) {
		p[i] = normprob(u,s,x[i]);
	}
	return p;
}

//std::vector<double> normpdf(const std::vector<int>&,double,double);
//std::vector<double> normpdf(const std::vector<double>&,double,double);
//	// eval_at, mean, var

std::vector<size_t> randset(int const&, std::vector<double> const&, std::mt19937&);

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

std::vector<double> normalize_probvec(std::vector<double>);

