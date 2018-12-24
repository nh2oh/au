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


