#include <vector>
#include "au_algs.h"

bool isany(std::vector<bool> v) {
	for (auto const& cv : v) {
		if (cv) { return true; }
	}
	return false;
}

bool isall(std::vector<bool> v) {
	for (auto const& cv : v) {
		if (!cv) { return false; }
	}
	return true;
}

std::vector<int> bool2idx(std::vector<bool> v_in) {
	std::vector<int> idx_true {};
	for (int i=0; i<v_in.size(); ++i) {
		if (v_in[i]) {
			idx_true.push_back(i);
		}
	}
	return idx_true;
}
