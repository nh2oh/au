#pragma once
#include "..\types\nv_t.h"
#include "..\types\ts_t.h"
#include "..\nv_uih.h"
#include <vector>
#include <optional>
#include <chrono>
#include <string>
#include <set>


// Fwd declarations from rp_t.h
//class nv_t;
//class ts_t;
//class bar_t;

struct rand_rp_opts {
	std::chrono::seconds maxt;
};

struct randrp_input {
	ts_t ts {};
	std::set<nv_t> nvset {};
	std::vector<double> pd {};
	int n_nts {0};
	bar_t n_bars {0};
};

struct randrp_input_check_result {
	bool is_valid {false};
	std::string msg {};
};

randrp_input_check_result rand_rp_check_input(randrp_input const);

std::optional<std::vector<nv_t>> rand_rp(ts_t,std::vector<nv_t>,
	std::vector<double>,int,bar_t);
std::optional<std::vector<nv_t>> rand_rp(ts_t,std::vector<nv_t>,
	std::vector<double>,int,bar_t,rand_rp_opts);





/*
// rand_rp user input helper
//
// The purpose of this class is to provide a container to hold uih-objects
// created from user-input, say, on a gui form, and to make decisions about
// whether or not it is valid and can be parsed to the input for rand_rp().  
//
//
class randrp_uih {
public:
	randrp_uih();
	//randrp_uih(randrp_input_uih const&);
	randrp_uih(ts_uih const&, std::vector<nv_uih> const&, 
		std::vector<double> const&, int const&, bar_t const&);

	void update(ts_uih const&, std::vector<nv_uih> const&, 
		std::vector<double> const&, int const&, bar_t const&);
	bool is_valid() const;
	int flags() const;
	randrp_input get() const;
private:
	struct randrp_input_uih {
		ts_uih ts {};
		std::vector<nv_uih> nvset {};
		std::vector<double> pd {};
		int n_nts {0};
		bar_t n_bars {0};
	};

	bool is_valid_ {false};
	std::string msg_ {""};
	int flags_ {0};
	randrp_input_uih last_ {};
		// The argument passed to the most recent call to update(), or that
		// passed to the constructor if update() has not yet been called.
		// Allows the object to decide if any change has been made and make a
		// decision if further processing is needed.  

	std::optional<randrp_input> randrp_input_ {};
		// If last_ is valid input for rand_rp(), rrp_input_ is that value.  
		// std::optional is used to prevent imposing some sort of "default" 
		// on a user of the randrp_uih class.  There's a chance that the 
		// randrp_uih object is created with an "invalid" input.  This is ok
		// where the argument to the constructor comes from a user.  
};

*/