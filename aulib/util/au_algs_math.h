#pragma once
#include <vector>
#include <cmath> // std::abs for template roundstep(), std::round for template roundquant()
#include <random>
#include <optional>

// Regression
struct linreg_result {
	double slope {0.0};
	double intercept {0.0};
	double regcoef {0.0}; // r-squared
};
std::optional<linreg_result> linreg(std::vector<double> const&, std::vector<double> const&);


// Floating point equality
bool isapproxint(double,int); // is approximately integer
bool isapproxeq(double, double, int);
bool ismultiple(double, double, int); // true if arg1/arg2 == integer

// Kahan summation
template<typename T>
struct ksum {
	T value {};
	T c {};

	ksum& operator+=(const T& rhs) {
		T y = rhs - c;
		T t = value + y;
		c = (t - value) - y;
		value = t;

		return *this;
	};
};


// Better version of isapproxeq()
template<typename T>
bool aprx_eq(T a, T b, int ulp=2) {
	auto e = std::numeric_limits<T>::epsilon();
	auto m = std::numeric_limits<T>::min();
	auto d = std::abs(a-b);
	auto s = std::abs(a+b);
	return (d <= e*s*ulp || d < m);
};

// Better version of isapproxint()
template<typename T>
bool aprx_int(T a, int ulp=2) {
	T ra {std::round(a)};
	return aprx_eq(a, ra, ulp);
};

template<typename T>
bool is_mersenne(T a, int ulp=2) {
	if (a < 1) {return false;}
	return aprx_int(std::log2(a+1),ulp);
};

// Vectorized lcm, gcd
int lcm(std::vector<int> const&);
int gcd(std::vector<int> const&);


// subject-to-round, allowed-round-targets
// allowed_round_targets _must_ be sorted smallest -> largest
template<typename T> T roundset(T const&, std::vector<T> const&);

// subject-to-round, allowed-round-targets
// allowed_round_targets _must_ be sorted smallest -> largest
template<typename T>
T roundset(T const& subject, std::vector<T> const& targets) {
	for (auto i=1; i<targets.size(); ++i) {
		auto s_t = (subject-targets[i]);
		if (s_t < (subject-subject)) {
			s_t = (targets[i]-subject);
		}
		auto s_tp = (subject-targets[i-1]);
		if (s_tp < (subject-subject)) {
			s_tp = targets[i-1]-subject;
		}

		if (s_t > s_tp) {
			return targets[i-1];
		}
	}
	return targets.back();
};


// vector of values to fuzz, random-fraction-of-value to add to each element
template<typename T> std::vector<T> fuzzset(std::vector<T> const&, double const&);

// vector of values to fuzz, random-fraction-of-value to add to each element
template<typename T>
std::vector<T> fuzzset(std::vector<T> const& v, double const& fuzz_frac) {
	std::vector<double> rns = urandd(v.size()+1,-fuzz_frac,fuzz_frac);
	auto v_fuzzed = v;
	for (int i=0; i<v.size(); ++i) {
		v_fuzzed[i] = v_fuzzed[i] + (v_fuzzed[i])*(rns[i]);
	}
	return v_fuzzed;
};

// subject-to-round, step
template<typename T> T roundquant(T const&, T const&);

// subject-to-round, step
template<typename T>
T roundquant(T const& subject, T const& step) {
	auto n = std::round((subject/step));
	return std::abs(n)*step;
};


// distance; always >= 0
template<typename T> T dist(T const&, T const&);

// distance; always >= 0
template<typename T>
T dist(T const& a, T const& b) {
	if (a < b) {
		return (b-a);
	}
	return (a-b);
};


// subject, set
template<typename T> T nearest(T const&, std::vector<T> const&);

// subject, set
template<typename T>
T nearest(T const& subject, std::vector<T> const& set) {
	T nearest_elem = set[0];
	T d_nearest = dist(subject,set[0]);
	for (int i=0; i<set.size(); ++i) {
		auto d = dist(subject,set[i]);
		if (d < d_nearest) {
			nearest_elem = set[i];
			d_nearest = d;
		}
	}
	return nearest_elem;
};

// subject, set
template<typename T> int nearest_idx(T const&, std::vector<T> const&);

// subject, set
template<typename T>
int nearest_idx(T const& subject, std::vector<T> const& set) {
	int nearest_elem_idx = 0;
	T d_nearest = dist(subject,set[nearest_elem_idx]);
	for (int i=0; i<set.size(); ++i) {
		auto d = dist(subject,set[i]);
		if (d < d_nearest) {
			nearest_elem_idx = i;
			d_nearest = d;
		}
	}
	return nearest_elem_idx;
};
