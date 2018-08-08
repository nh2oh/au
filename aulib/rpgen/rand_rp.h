#pragma once
#include <vector>
#include <optional>
#include <chrono>
#include <string>
#include "..\types\note_value_t.h"
#include "..\types\ts_t.h"
#include "..\types\beat_bar_t.h"

// Fwd declarations from rp_t.h
//class note_value;
//class ts_t;
//class bar_t;

struct rand_rp_opts {
	std::chrono::seconds maxt;
};

std::optional<std::vector<note_value>> rand_rp(ts_t,std::vector<note_value>,
	std::vector<double>,int,bar_t);
std::optional<std::vector<note_value>> rand_rp(ts_t,std::vector<note_value>,
	std::vector<double>,int,bar_t,rand_rp_opts);

struct rand_rp_input_helper {
	bool is_valid {false};
	std::string msg {};
	int flags {};

	ts_t ts {"4/4"_ts};
	std::vector<note_value> nvset {};
	std::vector<double> pd {};
	int nnts {};
	bar_t n_bars {};
};

rand_rp_input_helper validate_rand_rp_input(ts_t const&, std::vector<note_value> const&,
	std::vector<double> const&, int const&, bar_t const&);

