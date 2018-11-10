#pragma once
#include "scale.h"
#include "..\types\frq_t.h"
#include <vector>
#include <string>
#include <array>

//
// The problem with defining the scd_t/rscd_t as a global 
// type outside of all scales is that scd => rscd,oct => npc,oct/ntl,oct/ntstr,
// and scales can then no longer dynamically control the ordering or 
// categorization of notes, since scd_t's are ordered in an obvious and 
// transparent way.  
// An scd_t should be analagous to an iterator.  It should be an iterator into
// a scale object that can be incremented, decremented, and "dereferenced" to
// yield a concrete, soundable, scale-independent representation of a note.  Since 
// it's essentially a pointer to a scale object, the note returned can depend
// on the state of a scale instance at the time it is dereferenced.  
//
// A "note" (obtained by dereferencing an scd_t) has two characteristics:
// 1)  It is soundable:  it contains/can be trivially converted to a frq_t.
// 2)  It is printable:  it has a std::string representation.
// Both of these properties must be accessible w/o passing through any scale-
// dependent functionality.  Ie, both peoperties must be present in the note_t
// object itself.  Equality and ordering on these note_t's... depends on == frq_t's?
// ntl_t's?  both?  It makes no sense to leave it up to the scale, if a note_t 
// is supposed to be useful w/o the scale.  
// So then what's the point of embedding the note_t into the scale?  It should
// not need access to any scale-dependent functionality.  We are brought back to 
// something close to the initial design, in which ntl_t,ntstr_t were scale-
// independent.  
//
// * A scale is expected to provide an scd_t which dereferences to a scale-independent 
//   note_t.
//
//
// Users will show up with sets of numbers (~ 'scds') obtained from say a 
// notefile and will want to define eqiuv relations on these... they will
// want to 'bin' them into a finite number of categories... how should this
// work?  Should all scales be forced to define equiv relations on the 'global' 
// note_t ~ {ntl_t,frq_t} classes?  What should the rules be?  
// One easy way is for scales to do this implictly, by setting the ntl_t and
// oct_t of the note_t.  The user can define an equiv relation on the ntl_t.  
// The scale is free to choose to define the rules by which certain frq_t's are
// paired w/ ntl_ts in whatever wacky way it wants.  A scale not wanting to
// have any equiv relations can simply use numbers for the ntl_t's and give
// all note_t's an oct_t of 0.  
//
// A scale can always come along w/ its own brand new type and offer conversions
// of scd_t's, external note_t's etc to this new type.  The new type can 
// have whatever unusual equiv relation the scale wants to define.  
//
// How about:
// A scale _is_ required to define npc's, and these must be static (why? 
// --because i want nlt_t's, scd_t's to be classifyable into pitch classes 
// without having to instantiate an instance of a sc object which might have
// state, etc),
// with static (why?) == relations, but it is _not_ required that this set be 
// finite.  
// Consider what if i want a microtonal scale which allows ntl's like C'''''' where
// each ' is an ulp of whatever fundamental type frq is represented in.  In
// reality, a user who wants a finite set of npc's needs to check beforehand
// if the scale provides this.  For the case of ks_key(), the user knows 
// what the 12 npc's are; it's in the doc for the scale.  The usage can be
// something like:  
//
// spn12tet3 my_sc {};
// spn12tet::scd_t my_scd {my_sc,int};
// scpn12tet::npc_t my_npc {*my_scd};
//
//


class note_t {
	// Note that all members are "absolute;" that is, they do not reference
	// scale state or scale-dependent types.  However, despite this, the 
	// class is a sub-class of the scale.  To compare w/ note_t's that are
	// subclasses of different scales, someone will have to define a conversion
	// function/constructor/whatever.  Probably such a thing will go through
	// a scale-dependent validation function that parses the ntstr and frq.  
	//
	// Note that the ntl set, not the note_t class, is a static member of the scale
	// class.  This could probably go either way.  For now i want to keep all
	// the authority of deciding equality, membership, etc in the hands of the
	// scale.  Note that these methods need not be static.  Conversion from a
	// note_t to an npc_t is static, but conversion from a string/char* to a
	// note is not.  
	//
public:
	explicit note_t()=default;
	explicit note_t(const std::string&);

	// see spn12tet::isinsc()
	//bool is_valid(const std::string&) const;
	//bool is_valid(const frq_t&) const;
	//bool is_valid(const std::string&, const frq_t&) const;

	std::string print() const;
	frq_t frq() const;

	frq_t m_frq {252_Hz};
	ntl_t m_ntl {"C"};

	// A nod to the fact that the vast-vast majority of scales will have 
	// repeating ntl_t's
	oct_t m_oct {0};
private:
	// Turns out a note_t has no invariants!
};


class spn12tet3 {
public:
	class scd3_t {
		// The iterator for the scale.
		// Note here i include an int; this is an implementation detail of the
		// scd_t for this particular scale.  In the general case a sc need not
		// define its scd_t as containign an int, or even a ptr to a scale object.
		// For spn12tet3 i don't need the ptr, only the int, since the scale is
		// not dynamic.  
	public:
		explicit scd3_t();
		explicit scd3_t(int);

		note_t operator*() const;
		int operator-(const scd3_t&) const;
		scd3_t& operator++();  // prefix
		scd3_t operator++(int);  // postfix
		bool operator==(const scd3_t&) const;
		bool operator>(const scd3_t&) const;
		bool operator<(const scd3_t&) const;
	private:
		int m_val {0};
		// spn12tet3 *m_sc {};
	};

	

	// Constructors
	explicit spn12tet3()=default;  // Generates A440 ("A(4)" == 440 Hz)
	explicit spn12tet3(spn12tet3::pitch_std);

	// Getters
	std::string name() const;
	std::string description() const;
	std::string print() const;
	std::string print(spn12tet3::scd3_t,spn12tet3::scd3_t) const;

	// For note-letters derrived from different scales, the user is responsible
	// for converting their note_t to a string.  This makes it obvious that there
	// are no shenanigans.  That is, it is clear that the comparison only takes 
	// into account the printable ("externally observable") representation of the
	// ntl.  Alternatively, whatever note_t the user is working with can define its
	// own converting constructor to a spn12tet::note_t.  
	bool isinsc(const ntl_t&) const;
	bool isinsc(const frq_t&) const;
	bool isinsc(const note_t&) const;
	
	spn12tet3::scd3_t to_scd(const std::string&) const;
	spn12tet3::scd3_t to_scd(const spn12tet3::note_t&) const;
	std::vector<spn12tet3::scd3_t> to_scd(const std::vector<spn12tet3::note_t>&) const;
private:

	struct ntstr_parsed {
		bool is_valid {false};
		std::array<char,2> ntl {};
		int ntl_idx {0};
		int n_sharp {0};
		int n_flat {0};
		int oct {0};
		bool oct_specified {false};
	};
	static ntstr_parsed parse_ntstr(const std::string&);

	spn12tet3::note_t to_note(const std::string&);  // NB not nec const...
	spn12tet3::note_t to_note(const frq_t&);  // NB not nec const...

	pitch_std m_pstd {};
	int m_shift_scd {57};  // the scd that generates the ref frq; 57 => A(4)
	
	std::vector<ntl_t> m_ntls {"C"_ntl,"C#"_ntl,"D"_ntl,"D#"_ntl,
		"E"_ntl,"F"_ntl,"F#"_ntl,"G"_ntl,"G#"_ntl,"A"_ntl,"A#"_ntl,"B"_ntl};

	std::string m_name {"12-tone chromatic"};
	std::string m_description {"12-tone equal-tempered with A(4)=440Hz; SPN"};
};

/*
bool operator!=(const spn12tet3::ntl3_t&, const spn12tet3::ntl3_t&);
bool operator>(const spn12tet3::ntl3_t&, const spn12tet3::ntl3_t&);
bool operator<=(const spn12tet3::ntl3_t&, const spn12tet3::ntl3_t&);
bool operator>=(const spn12tet3::ntl3_t&, const spn12tet3::ntl3_t&);
*/





