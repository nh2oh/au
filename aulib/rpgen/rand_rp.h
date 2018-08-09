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




struct randrp_input {
	ts_t ts {beat_t{1.0}, note_value{1.0/4.0}, 0};
	std::vector<note_value> nvset {};
	std::vector<double> pd {};
	int n_nts {0};
	bar_t n_bars {0};
};

// Note-value user input helper
//
// The purpose of this class is to provide a container to hold std::string
// input entered by a user, say, on a gui form, and to make decisions about
// whether or not it is valid and can be parsed to a note_value.  
//
//
class randrp_uih {
public:
	randrp_uih();
	randrp_uih(randrp_input const&);

	void update(randrp_input const&);
	bool is_valid() const;
	int flags() const;
	randrp_input get() const;
private:
	std::optional<nv_str_parts> parse_randrp_input_() const;
		// Parses str_last_; does not set any internal variables (note the const
		// declaration).  Called by the constructor and by update().  

	bool is_valid_ {false};
	std::string msg_ {""};
	int flags_ {0};
	randrp_input last_ {};
		// The argument passed to the most recent call to update(), or that
		// passed to the constructor if update() has not yet been called.
		// Allows the object to decide if any change has been made and make a
		// decision if further processing is needed.  

	std::optional<randrp_input> rrp_input_ {};
		// If last_ is valid input for rand_rp(), rrp_input_ is that value.  
		// std::optional is used to prevent imposing some sort of "default" 
		// on a user of the randrp_uih class.  There's a chance that the 
		// randrp_uih object is created with an "invalid" input.  This is ok
		// where the argument to the constructor comes from a user.  
};












//
//struct rand_rp_input_helper {
//	bool is_valid {false};
//	std::string msg {};
//	int flags {};
//
//	ts_t ts {"4/4"_ts};
//	std::vector<note_value> nvset {};
//	std::vector<double> pd {};
//	int nnts {};
//	bar_t n_bars {};
//};
//
//rand_rp_input_helper validate_rand_rp_input(ts_t const&, std::vector<note_value> const&,
//	std::vector<double> const&, int const&, bar_t const&);

