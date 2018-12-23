#pragma once
#include <vector>
#include <cmath> // std::abs for template roundstep(), std::round for template roundquant()
#include <random>
#include <optional>
#include <type_traits>
#include <utility>

template<typename Tf, typename Ts>
std::vector<Tf> vprod(std::vector<Tf> a, std::vector<Ts> b) {
	if (a.size() != b.size()) {
		std::abort();
	}
	std::vector<Tf> res {};  res.reserve(a.size());
	for (size_t i=0; i<a.size(); ++i) {
		res.push_back(a[i]*b[i]);
	}
	return res;
}

template<typename Tf, typename Ts, typename... Trest>
std::vector<Tf> vprod(std::vector<Tf> a, std::vector<Ts> b, Trest... rest) {
	if (a.size() != b.size()) {
		std::abort();
	}
	std::vector<Tf> res {};  res.reserve(a.size());
	for (size_t i=0; i<a.size(); ++i) {
		res.push_back(a[i]*b[i]);
	}
	return vprod(res,rest...);
}




