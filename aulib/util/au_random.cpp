#include "au_random.h"
#include "au_util.h"
#include <vector>
#include <random>
#include <chrono> // To generate a seed for the random engine
#include <numeric>  // accumulate(), partial_sum() (not in <algorithm>)
#include <algorithm>  // find_if()
#include <exception>

// Get a new random number generator engine
// TODO:  t does not have the resolution to do this correctly... is it seconds????
// If randseed == true, the engine is seeded with the least-significant digits
// of the number of seconds (???) between the present time and the system 
// clock's (???) epoch.  If false, the engine is created w/ the default seed. 
std::mt19937 new_randeng(bool randseed) {
	std::default_random_engine randeng {};
	auto t = std::chrono::system_clock::now().time_since_epoch().count();
	auto least_sig = static_cast<int>(t % std::numeric_limits<int>::max());
	if (randseed) {
		randeng.seed(least_sig);
	}
	return randeng;
}


// Random ints (urandi), doubles (urandd)
// Return a vector of n random ints on [min, max] chosen from a uniform 
// distribution.  A new random engine is instantiated and seeded with
// new_randeng(true).  
std::vector<int> urandi(int n, int min, int max) {
	auto re = new_randeng(true);
	std::uniform_int_distribution<int> rn {min,max};
	std::vector<int> rv(n,0);
	for (auto i=0; i<n; ++i) {
		rv[i] = rn(re);
	}
	return rv;
}
std::vector<int> urandi(int n, int min, int max, std::mt19937& re) {
	std::uniform_int_distribution<int> rn {min,max};
	std::vector<int> rv(n,0);
	for (auto i=0; i<n; ++i) {
		rv[i] = rn(re);
	}
	return rv;
}
/*
std::vector<int> urandi(int n, int min, int max) {
	auto re = new_randeng(true);
	std::uniform_int_distribution<int> rn {min,max};
	std::vector<int> rv(n,0);
	for (auto i=0; i<n; ++i) {
		rv[i] = rn(re);
	}
	return rv;
}*/

/*
double normprob(double mean, double stdev, double x) {
	double pi {3.14159265358979323846};
	double pre = 1.0/(stdev*std::sqrt(2.0*pi));  // "prefactor"
	double e = (-0.5)*std::pow(((x-mean)/stdev),2);  // "exponent"

	return pre*std::exp(e);
}*/

/*std::vector<double> normpdf(const std::vector<int>& x, double mean, double stdev) {
	std::vector<double> p(x.size(),0.0);
	for (int i=0; i<x.size(); ++i) {
		p[i] = normprob(mean,stdev,x[i]);
	}
	return p;
}*/



std::vector<double> urandd(int n, double min, double max) {
	auto re = new_randeng(true);
	return urandd(n, min, max, re);
}
std::vector<double> urandd(int n, double min, double max, std::mt19937& re) {
	std::uniform_real_distribution<double> rn {min,max};
	std::vector<double> rv(n,0.0);
	for (auto i=0; i<n; ++i) {
		rv[i] = rn(re);
	}
	return rv;
}

std::vector<double> normalize_probvec(const std::vector<double>& p) {
	double sum_p {0.0};
	for (const auto& e : p) {
		if (e < 0) { std::abort(); }
		sum_p += e;
	}
	
	std::vector<double> result = p;
	if (sum_p == 0.0) { return result; }  // means all elements of p == 0
	std::for_each(result.begin(),result.end(),[sum_p](double& e){ e /= sum_p; });

	return result;
}


