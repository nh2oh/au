#pragma once
#include "util/au_util_all.h"
#include <string>
#include <optional>

// User input helpers for "simple" numbers
//
// The purpose of this class is to provide a container to hold std::string
// input entered by a user, say, on a gui form, and to make decisions about
// whether or not it is valid and can be parsed to a number.  
//
// T ~ double, int, ...
//

class uihpred_is_gt_zero {
public:
	bool operator() (int const& int_in) const {
		return (int_in > 0);
	};
};

class uihpred_is_geq_zero {
public:
	bool operator() (int const& int_in) const {
		return (int_in >= 0);
	};
};

class int_uih {
public:
	int_uih();
	int_uih(std::string const&);
	// number_uih(std::string const&, predicates...);  Future...
	// Also possibly some way to set custom messages, flags dfns...

	void update(std::string const&);
	bool is_valid() const;
	int flags() const;
	int get() const;

	bool operator==(int_uih const&) const;
private:
	std::optional<int> parse_str_() const;
	// Parses last_; does not set any internal variables (note the const
	// declaration).  Called by the constructor and by update().  

	bool is_valid_ {false};
	std::string msg_ {""};
	int flags_ {0};
	std::string last_ {""};
	// The std::string passed to the most recent call to update(), or that
	// passed to the constructor if update() has not yet been called.
	// Allows the object to decide if any change has been made and make a
	// decision if further processing is needed.  

	std::optional<int> number_ {};
	// If str_last_ can be parsed to create a number, number_ is that number.  
	// std::optional is used to prevent imposing some sort of "default" ts
	// on a user of the number_uih class.  Since the constructor takes a 
	// string, not a number object, there's a chance that the number_uih is
	// created with an "invalid" input.  This is ok where the argument to
	// the constructor comes from a user.  
};

int_uih::int_uih() {
	//...
};

int_uih::int_uih(std::string const& str_in) {
	update(str_in);
};

void int_uih::update(std::string const& str_in) {
	if (str_in == last_) {
		return;
	}
	last_ = str_in;
};

bool int_uih::is_valid() const {
	return is_valid_;
};

int int_uih::flags() const {
	return flags_;
};

int int_uih::get() const {
	au_assert(is_valid_, "Call to int_uih::get() where is_valid_ == false.");
	return *number_;
}

bool int_uih::operator==(int_uih const& rhs) const {
	//...
	return false;
};