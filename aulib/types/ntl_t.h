#pragma once
#include "scd_t.h"  // declares/defines octn_t
#include <string>


class ntl_t {
public:
	ntl_t() = default;
	explicit ntl_t(const char*);
	explicit ntl_t(std::string const&);

	std::string print() const;

	friend bool operator==(ntl_t const&, ntl_t const&);
	friend bool operator!=(ntl_t const&, ntl_t const&);
private:
	void set_ntl(std::string const&); // Delegated constructor

	std::string m_ntl {"C"};
	static const std::string m_illegal;  // Chars not allowed in an ntl_t
};

ntl_t operator""_ntl(const char *, size_t);

//-----------------------------------------------------------------------------
// The ntstr_t class
class ntstr_t {
public:
	ntstr_t() = default;
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

