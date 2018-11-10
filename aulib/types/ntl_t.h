#pragma once
#include "scd_t.h"  // declares/defines octn_t
#include <string>

class ntl_t {
public:
	explicit ntl_t() = default;
	explicit ntl_t(const char*);
	explicit ntl_t(const std::string&);

	std::string print() const;

	bool operator==(const ntl_t&) const;
private:
	void set_ntl(const std::string&);  // Delegated constructor

	std::string m_ntl {"C"};
};

bool operator!=(const ntl_t&, const ntl_t&);

ntl_t operator""_ntl(const char*, size_t);

//-----------------------------------------------------------------------------
// The ntstr_t class
class ntstr_t {
public:
	explicit ntstr_t() = default;
	explicit ntstr_t(std::string);
	explicit ntstr_t(std::string,int);
	explicit ntstr_t(ntl_t, int);  // arg2 ~ octave number
	explicit ntstr_t(ntl_t, octn_t);

	std::string print() const;

	const octn_t& oct() const;
	const ntl_t& ntl() const;
	explicit operator ntl_t() const;
	bool operator==(const ntstr_t&) const;
private:
	void from_string(const std::string&);
	ntl_t m_ntl {"C"};
	octn_t m_octn {0};
};

bool operator!=(const ntstr_t&, const ntstr_t&);




