#pragma once
#include <vector>
#include <optional>
#include <chrono>

// Fwd declarations from rp_t.h
class note_value;
class ts_t;
class bar_t;

struct rand_rp_opts {
	std::chrono::seconds maxt;
};

std::optional<std::vector<note_value>> rand_rp(ts_t,std::vector<note_value>,\
	std::vector<double>,int,bar_t);
std::optional<std::vector<note_value>> rand_rp(ts_t,std::vector<note_value>,\
	std::vector<double>,int,bar_t,rand_rp_opts);
