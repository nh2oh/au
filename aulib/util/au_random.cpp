#include "au_random.h"
#include "au_error.h"
#include "au_util.h"
#include <vector>
#include <random>
#include <chrono> // To generate a seed for the random engine
#include <numeric>  // accumulate(), partial_sum() (not in <algorithm>)
#include <algorithm>  // find_if()

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

std::vector<double> normalize_probvec(std::vector<double> p) {
	if (p.size() == 0) {
		return p;
	}
	//au_assert(p.size()>0, "p>0");
	au_assert(!std::any_of(p.begin(),p.end(),[](double const& e){ return e<0; }),
		"all elements of p must be >= 0");

	auto sum_p = std::accumulate(p.begin(),p.end(),0.0);
	if (sum_p == 0.0) {
		return p;
	}

	auto pnorm = p;
	std::for_each(pnorm.begin(),pnorm.end(),[sum_p](double& e){ e /= sum_p; });

	return pnorm;
}

// Random integers on [0,p.size()).  
// The probability of i is proportional to the value p[i].  If p is a vector
// of zeros, all values in the range are drawn with equal probability.  p
// must not contain any values < 0.  If the values in p sum to > 1, p is
// normalized before generating the set.  
// That is, p == {1,2} => p == {1/3,2/3}
std::vector<size_t> randset(int const& n, std::vector<double> const& p,
	std::mt19937& re) {
	au_assert(p.size()>0, "p>0");
	au_assert(!std::any_of(p.begin(),p.end(),[](double const& e){ return e<0; }),
		"all elements of p must be >= 0");

	auto cp = std::vector<double>(p.size(), 0.0);  // will hold the cumulative
												   // probability
	auto sum_p = std::accumulate(p.begin(),p.end(),0.0);
	if (sum_p > 0.0) {
		cp = p;
		std::for_each(cp.begin(),cp.end(),[sum_p](double& e){ e /= sum_p; });
	} else {
		// sum_p == 0 => all elements of p == 0, since there are no (-) values
		// allowed in p.  This check has to be made to avoid division by zero
		// in the call to divide() when normalizing the probability vector.  
		//
		// A vector of 0's means the user wants all the probabilities to be
		// the same.  
		cp = std::vector<double>(p.size(), 1.0/p.size());
	}
	std::partial_sum(cp.begin(),cp.end(),cp.begin());  // cumulative sum

	std::uniform_real_distribution<double> rn {0,1};

	std::vector<size_t> ridx {}; ridx.reserve(n);
	for (int i=0; i<n; ++i) {
		auto rd = rn(re);  // "random double"

		auto cridx = std::find_if(cp.begin(),cp.end(),
			[rd](double const& e){return e>=rd;})-cp.begin();
		ridx.push_back(cridx);
	}
	return ridx;
}




