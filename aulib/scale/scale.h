#pragma once
#include "..\types\ntl_t.h"
#include "..\types\frq_t.h"
#include "..\types\scd_t.h"  // octn_t
#include <vector>


struct pitch_std {
	explicit pitch_std()=default;
	// These control the pitch standard, ie, the actual frq values of the 
	// named pitches of SPN (C(i),C#(i),D(i),...B(i),C(i+1),...).  
	// Changing ref_note here does _not_ change the note @ which the 
	// octave boundry occurs.  
	note_t ref_note {ntl_t{"A"}, octn_t{4}, frq_t{440}};
	int gen_int {2};
	int ntet {12};
};

// ref-frq, ntet, gint
std::vector<frq_t> frq_eqt(const std::vector<int>&,frq_t,int,int);

// scd (dn), ref-frq, ntet, gint
frq_t frq_eqt(int, frq_t,int,int);

// ref-frq, ntet, gint
double n_eqt(frq_t, frq_t,int,int);


//
// Random notes about scales
//
// The problem with defining the scd_t/rscd_t as a "global" type outside of all scales 
// is that an scd => sn rscd,oct => an npc,oct/ntl,oct/ntstr, and scales can then no
// longer dynamically control the ordering or categorization of notes, since "global" 
// scd_t's are ordered in an obvious and transparent way.  
//
// An scd_t should be analagous to an iterator.  It should be an iterator into
// a scale object that can be incremented, decremented, and "dereferenced" to
// yield a concrete, soundable, scale-independent representation of a note.  Since 
// it's essentially a pointer to a scale object, the note returned can depend
// on the state of a scale instance at the time it is dereferenced; dereferencing an scd_t
// may change the result of dereferencing other, extant scd_t's; it's up to the scale.  
//
// A "note" (obtained by dereferencing an scd_t) has two characteristics:
// 1)  It is soundable:  it contains/can be trivially converted to a frq_t.
// 2)  It is printable:  it has a std::string representation.
// Both of these properties are be accessible w/o passing through any scale-
// dependent functionality.  Ie, both functionalities are inherent in the note_t
// object itself.  Equality and ordering on these note_t's... depends on == frq_t's?
// ntl_t's?  both?  It makes no sense to leave it up to the scale, if a note_t 
// is supposed to be useful w/o the scale.  It's up to the user.  
//
// A downside to this design is that since note_t's don't "belong" to scales they 
// can be used "unsafely" between multiple scales.  That is, a user can take a note_t
// obtained from scale a and try to use it to obtain an scd from scale b, but the
// note may not be valid on scale b.  
//
// * A scale is expected to provide an scd_t which dereferences to a scale-independent 
//   note_t.
//
// Users will show up with sets of numbers (~ 'scds') obtained from say a notefile and 
// will want to define eqiuv relations on these... they will want to 'bin' them into a 
// finite number of categories... how should this work?  Should all scales be forced to 
// define equiv relations on the 'global' (ie, non-member) note_t ~ {ntl_t,frq_t} classes?  
// What should the rules be?  
//
// One easy way is for scales to do this implictly, by setting the ntl_t and
// oct_t of the note_t.  One possible equiv relation for sets of notes is based on the
// operator== for ntl_t.  
//
// The scale is free to choose to define the rules by which certain frq_t's are
// paired w/ ntl_ts in whatever wacky way it wants.  A scale not wanting to
// have any equiv relations can simply use numbers for the ntl_t's and give
// all note_t's an oct_t of 0.  
//
// A scale can always come along w/ its own brand new type and offer conversions
// of scd_t's, external note_t's etc to this new type.  The new type can 
// have whatever unusual equiv relation the scale wants to define.  
//
// * Scales assign the note_t's (returned by operator* on their scd_t iterators) to
//   pitch classes _implictly_ with the ntl_t field in the note_t.  Note pitch classes 
//   are therefore defined by note_t.ntl_t.  Of course, a scale wanting to define different
//   types of pitch classes are free to do so.  A scale is free to convert its scd_t's 
//   to anything they want, say:  
//   blorg_t sc.to_blorg_pitch_class(sc::scd3_t),
//   blorg_t sc.to_blorg_pitch_class(note_t)
//   etc
//
// A scale can avoid this implicit assignment of pitch classes by not reusing ntl_t's.
// For example, consider a "microtonal" scale where every increment of any given scd
// dereferences to a note_t w/a frq 1 ulp of a std::double above the previous scd (scd--).
// The scale can just name the notes as "n[i]" where i is some int.  Users who want a 
// finite set of npc's/ntl_t's need to check the scale documntation beforehand, and choose
// an appropriate scale.  For the case of ks_key():  The algorithm is only meaningfull on
// spn12tet the npc's/ntl_t's for which are well documented an widely known.  it is not
// reasonable to expect to be able to plug any arbitrary scale into this function.  
//
// BIG PICTURE:  All scales sc offer an sc.to_scd() for note_t, frq_t, ntl_t, etc.
// The sc::scd_t returned represents the position of the input in the scale.  
//
// TODO:  Questions
// Are ctors of scd_t's public?  Private?  Up to the scale?  
// >,<, etc operators on scd_t's ??  Up to the scale?
//
//
// Users will also show up w/ sets of scd_t's from scale a and want to convert them to
// scale_b.  How should this be dealt with?  
// Scale a may define a converting operator from its scd_t to int; scale b may define the
// overload b.to_scd(int).  Scale b may also define a ctor b::scd_t(int,*instance_of_b).  
// If different scales define conversions between all other scales i lose the scale-independent
// property that scd_t's were supposed to provide in the first place.  Requiring conversion
// to int may allow for the best of both worlds.  
//
// How about:  
// scd_t's are distinguishable and ordered (convertible to int), start from 0 == sc.begin().  
// This allows conversion between different scales.  sc.begin() => a zero for the scale.  
// They have all the usual operators: ==,<,>,!=, etc based on their transparent int: "the 
// n'th scale degree is always the n'th scale degree, but its frq, ntl, etc may change (for
// a dynamic scale)."  
//
// Allow scd_t's to be constructed w/o a scale w/ ints?  User can call some sort of scd_t.set(*sc)
// method later on when ready to deref ?.  scd_t would have to be a template: scd_t<typename sc>.  
// What's the point of this?  Why not just have users work with naked ints?  No association to
// a scale... scd_t just goes back to being a "strong type."  
//
// All sc.to_scd() methods are const; sc::scd_t::operator*() may change the state of the 
// scale but the to_scd() methods do not.  
// This means that extracting a set of scd_t's from a dynamic scale then applying them to
// a different scale does not capture the dynamism of the first sc.  Might be unexpected.  
//

