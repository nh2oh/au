#pragma once
#include "frq_t.h"
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
	void set_ntl(const std::string&);  // Essentially a delegated constructor

	static const char *m_allowed;
	std::string m_ntl {"C"};
};

bool operator!=(const ntl_t&, const ntl_t&);

ntl_t operator""_ntl(const char*, size_t);



//
// The octn_t class
//
// An octn_t represents an absolute "octave _number_" on whatever scale the user
// is working with.  Since scales are free to define the concept of "octave" any 
// way they please, an octn_t in general has no relationship with frequency or
// frequency ratios (see oct_t for this).  
//
class octn_t {
public:
	octn_t() = default;
	explicit octn_t(int);

	int to_int() const;
	std::string print() const;

	bool operator==(const octn_t&) const;
	bool operator!=(const octn_t&) const;
	bool operator<(const octn_t&) const;
	bool operator<=(const octn_t&) const;
	bool operator>(const octn_t&) const;
	bool operator>=(const octn_t&) const;
	
private:
	int m_octn {0};
};



struct spn_ntstr_parsed {
	bool is_valid {false};
	bool is_oct_set {false};
	bool is_valid_spn {false};
	ntl_t ntl {};
	ntl_t ntl_base {};  // No sharp or flat modifiers
	int nsharp {0};
	int nflat {0};
	octn_t oct {};
};
spn_ntstr_parsed parse_spn_ntstr(const std::string&);

struct ntstr_parsed {
	bool is_valid {false};
	bool is_oct_set {false};
	ntl_t ntl {};
	octn_t oct {};
};
ntstr_parsed parse_ntstr(const std::string&);

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
	std::string print(note_t::fmt=note_t::fmt::ntlo) const;

	ntl_t ntl {"C"};
	octn_t oct {0};
	frq_t frq {16.352};

	bool operator==(const note_t&) const;
private:  // Turns out a note_t has no invariants!
};
bool operator!=(const note_t&, const note_t&);




