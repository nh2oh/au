#pragma once
#include <string>
#include "scd_t.h"


//-----------------------------------------------------------------------------
// The ntl_t class
//

class ntl_t {
public:
	ntl_t() = delete;
	explicit ntl_t(const char*);
	explicit ntl_t(std::string);

	std::string print() const;

	friend bool operator==(ntl_t const&, ntl_t const&);
private:
	void set_ntl(std::string); // Delegated constructor

	std::string m_ntl {"C"};
};

ntl_t operator""_ntl(const char *, size_t);

//-----------------------------------------------------------------------------
// The ntstr_t class
class ntstr_t {
public:
	ntstr_t() = delete;
	explicit ntstr_t(std::string);
	explicit ntstr_t(ntl_t, int);  // arg2 ~ octave number
	explicit ntstr_t(ntl_t, octn_t);

	std::string print() const;

	explicit operator octn_t() const;
	explicit operator ntl_t() const;

	friend bool operator==(ntstr_t const&, ntstr_t const&);
private:
	ntl_t m_ntl {"C"};
	octn_t m_octn {0};
};

