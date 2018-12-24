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


// keep
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

// keep
// Normal probability _density_ evaluated at each value of x.
template<typename T>
std::vector<double> normpdf(const std::vector<T>& x, T u, T s) {
	std::vector<double> p {};  p.reserve(x.size());
	for (int i=0; i<x.size(); ++i) {
		p.push_back(normprob(u,s,x[i]));
	}
	return p;
}

std::vector<double> normalize_probvec(const std::vector<double>&);


