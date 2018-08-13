#pragma once
#include "types/nv_t.h"
#include <string>
#include <optional>

/*
template<typename T>
struct uih_pred_t {
	bool operator()(T const&) {
		return true;
	}
	std::string msg {""};
};
*/


//
// Note-value user input helper
//
// The purpose of this class is to provide a container to hold std::string
// input entered by a user, say, on a gui form, and to make decisions about
// whether or not it is valid and can be parsed to a nv_t.  
//
//
class nv_uih {
public:
	nv_uih();
	nv_uih(std::string const&);

	void update(std::string const& str_in);
	bool is_valid() const;
	int flags() const;
	std::string message() const;
	std::optional<nv_t> get() const;

	friend bool operator==(nv_uih const&, nv_uih const&);
	friend bool operator!=(nv_uih const&, nv_uih const&);
private:
	struct nv_str_parts {
		double num {0.0};
		double denom {0.0};
		int ndots {0};
	};
	// In a ts_t, num and denom are ints, but the constructor takes the 
	// ratio, which is (obviously) a double, in general.  Storing the results
	// of the str parse as doubles avoids a messy typecast.  

	// Parses str_last_; does not set any internal variables (note the const
	// declaration).  Called by the constructor and by update(). 
	std::optional<nv_str_parts> parse_nv_str() const;
		 
	bool m_is_valid {false};
	std::string m_msg {""};
	int m_flags {0};
	std::string m_str_last {""};
		// The std::string passed to the most recent call to update(), or that
		// passed to the constructor if update() has not yet been called.
		// Allows the object to decide if any change has been made and make a
		// decision if further processing is needed.  

	std::optional<nv_t> m_nv {};
		// If m_str_last_ can be parsed to create a nv_t, nv_ is that value.  
		// std::optional is used to prevent imposing some sort of "default" ts
		// on a user of the nv_uih class.  Since the constructor takes a 
		// string, not an nv_t object, there's a chance that the nv_uih is
		// created with an "invalid" input.  This is ok where the argument to
		// the constructor comes from a user.  
};

