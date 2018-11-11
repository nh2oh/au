#pragma once
#include "scale.h"
#include "..\types\frq_t.h"
#include "..\types\ntl_t.h"
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
// A downside to this design is that note_t's don't "belong" to scales and can be
// used "unsafely" between multiple scales.  
//
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
// oct_t of the note_t.  The equiv relation for sets of notes is based on the
// operator== for ntl_t.  
// The scale is free to choose to define the rules by which certain frq_t's are
// paired w/ ntl_ts in whatever wacky way it wants.  A scale not wanting to
// have any equiv relations can simply use numbers for the ntl_t's and give
// all note_t's an oct_t of 0.  
//
// A scale can always come along w/ its own brand new type and offer conversions
// of scd_t's, external note_t's etc to this new type.  The new type can 
// have whatever unusual equiv relation the scale wants to define.  
//
// * Scales group the note_t's returned by operator* on their scd_t iterators
//   _implictly_ by the ntl_t field in the note_t.  Note pitch classes are defined
//   by note_t.ntl_t.  Scales are free to convert their scd_t's to anything else
//   they want, say:  
//   blorg_t sc.to_blorg_pitch_class(sc::scd3_t),
//   blorg_t sc.to_blorg_pitch_class(note_t)
//   etc
//
// BIG PICTURE:  All scales sc offer an sc.to_scd() for note_t, frq_t, ntl_t, etc.
// The sc::scd_t returned represents the position of the input in the scale.  
//
//
// How about:
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




// Note that all members are "absolute;" that is, they do not reference
// scale state or scale-dependent types. However, despite this, the
// class is a sub-class of the scale. To compare w/ note_t's that are
// subclasses of different scales, someone will have to define a conversion
// function/constructor/whatever. Probably such a thing will go through
// a scale-dependent validation function that parses the ntstr and frq.
//
//
class note_t {
public:
	//explicit note_t()=default;
	//explicit note_t(ntl_t,octn_t,frq_t);

	ntl_t ntl {"C"};
	octn_t oct {0};
	frq_t frq {252.0};
	
	// The vast-vast majority of scales will have a finite set of repeating ntl_t's,
	// and it is very useful to be able to number them.  Hence the oct field.  
	// Assignment of this field and its interpretation is left _completely_ up to 
	// the scale.  
private:  // Turns out a note_t has no invariants!
};


// Should this be a template param for the scale ?
struct pitch_std3 {
	explicit pitch_std3()=default;
	// These control the pitch standard, ie, the actual frq values of the 
	// named pitches of SPN (C(i),C#(i),D(i),...B(i),C(i+1),...).  
	// Changing ref_note here does _not_ change the note @ which the 
	// octave boundry occurs.  
	note_t ref_note {ntl_t{"A"}, octn_t{4}, frq_t{440}};
	int gen_int {2};
	int ntet {12};
};

class spn12tet3 {
public:
	class scd3_t {
	public:
		// Note these ctors are public... in general scale scd3_t's need not provide
		// public ctors...
		explicit scd3_t(int,const spn12tet3*);

		note_t operator*() const;
		scd3_t& operator++();  // prefix
		scd3_t operator++(int);  // postfix
		scd3_t& operator--();  // prefix
		scd3_t operator--(int);  // postfix
		scd3_t& operator-=(const scd3_t&);
		friend int operator-(const scd3_t&,const scd3_t&);
		//bool operator==(const scd3_t&) const;
		//bool operator>(const scd3_t&) const;
		//bool operator<(const scd3_t&) const;
	private:
		int m_val {0};
		const spn12tet3 *m_sc {};
	};
	

	// Constructors
	explicit spn12tet3()=default;  // Generates A440 ("A(4)" == 440 Hz)
	explicit spn12tet3(pitch_std3);

	// Getters
	std::string name() const;
	std::string description() const;
	std::string print() const;
	std::string print(int,int) const;

	bool isinsc(const ntl_t&) const;
	bool isinsc(const frq_t&) const;
	bool isinsc(const note_t&) const;
	
	spn12tet3::scd3_t to_scd(const ntl_t&, const octn_t&) const;
	spn12tet3::scd3_t to_scd(const note_t&) const;
	spn12tet3::scd3_t to_scd(const frq_t&) const;
	std::vector<spn12tet3::scd3_t> to_scd(const std::vector<note_t>&) const;
private:
	struct base_ntl_idx_t {
		bool is_valid {false};
		int ntl_idx {0};  // [0,m_ntls.size())
		int scd_idx {0};

		// Expect:  ntl_idx==(scd_idx+m_ntls.size())%(m_ntls.size);
	};

	// Methods
	base_ntl_idx_t base_ntl_idx(const ntl_t&, const octn_t&) const;
	base_ntl_idx_t base_ntl_idx(const frq_t&) const;

	note_t to_note(int) const;  // Getter called by scd3_t::operator*()
	
	// Data
	pitch_std3 m_pstd {};
	const int N {12};
	int m_shift_scd {57};  // the scd that generates the ref frq; 57 => A(4)
	const std::vector<ntl_t> m_ntls {"C"_ntl,"C#"_ntl,"D"_ntl,"D#"_ntl,
		"E"_ntl,"F"_ntl,"F#"_ntl,"G"_ntl,"G#"_ntl,"A"_ntl,"A#"_ntl,"B"_ntl};
	std::string m_name {"12-tone chromatic"};
	std::string m_description {"12-tone equal-tempered with A(4)=440Hz; SPN"};
};

