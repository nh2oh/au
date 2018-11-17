#pragma once
#include "scd_t.h"  // declares/defines octn_t
#include "frq_t.h"  // declares/defines octn_t
#include <string>

class ntl_t {
public:
	explicit ntl_t() = default;
	explicit ntl_t(const char*);
	explicit ntl_t(const std::string&);

	static bool valid_string(const std::string&);

	std::string print() const;

	bool operator==(const ntl_t&) const;
private:
	void set_ntl(const std::string&);  //  Essentially a delegated constructor

	static const char *m_allowed;
	std::string m_ntl {"C"};
};

bool operator!=(const ntl_t&, const ntl_t&);

ntl_t operator""_ntl(const char*, size_t);



// 
// A note_t is a concrete, absolute, standalone representation of a pitch that associates
// (1) a name (constrained only by the rules for ntl_t), (2) an octn_t, and (3) a frq_t.  
// A note_t's functionality does not depend on a scale.  
// 
// The octn ("octave number") is present in recognition of the fact that the vast-vast 
// majority of scales will have a finite set of repeating ntl_t's, and it is therefore very 
// useful to be able to number them.  How this field is assigned by a scale is completely up
// to the scale.  Likewise, its interpretation is up to the scale and/or the user of the
// note_t object.  
// 
// Operator==(const note_t&) returns true iff all three fields compare ==
//
class note_t {
public:
	explicit note_t()=default;
	explicit note_t(ntl_t, octn_t, frq_t);

	enum class fmt {
		ntl,
		ntlo
	};
	std::string print(note_t::fmt) const;

	ntl_t ntl {"C"};
	octn_t oct {0};
	frq_t frq {16.352};

	bool operator==(const note_t&) const;
private:  // Turns out a note_t has no invariants!
};
bool operator!=(const note_t&, const note_t&);

struct ntstr_parsed {
	bool is_valid {false};
	bool is_oct_set {false};
	ntl_t ntl {};
	octn_t oct {};
};
ntstr_parsed parse_ntstr(const std::string&);




//-----------------------------------------------------------------------------
// The ntstr_t class
// TODO:  Deprecate
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




