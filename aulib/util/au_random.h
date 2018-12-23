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


