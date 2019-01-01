#pragma once
#include "nv_t.h"
#include "ts_t.h"
#include "beat_bar_t.h"
#include <vector>
#include <string>


//
// voice_t<T>
// 
// Represents a continuous sequence of "elements" that occupy horizontal space on a single staff.  
// Each element has a duration and is either a note or a rest.  
//
// To support rhythm calcs, associates a ts_t.  
// Chords are indicated implictly by multiple adjacent element_t's w/ the same ontime and d_t.  
// A rest is indicated by the field element_t.rest.  A rest is neither a d_t or a type of note 
// (it has duration so isn;t a member of any set of d_t's, however, it is not a member of any
// scale, so can't be considered a note).  Rests only arise in the context of _sequences_ of 
// notes/chords.  I don't know of any interesting features of rests that would compel me to make
// them a standalone type.  
// Possibility:  I could make the note field a union note_t, rest_t.  
// Force association with cchrom-spn & drop the templated note_t?  Essentially requires that the
// voice be printable in std notation.  
//
//
// In geneneral the problem with composing a billion different types into a billion different
// containers (ex: rp_t) is that when i need to put everything together, i have to pull the
// containers apart and make sequences of "elements," as above, to get data locality.  I have to
// repeat the code in all the containers i pulled apart in my container of "elements."  Ex,
// the voice_t will have to implement the same calcs as the rp_t container.  
//
//
template<typename T_note>
class voice_t {
public:
	struct element_t {
		beat_t on {0};
		d_t d {d::z};
		T_note note {};  // whatever representation of a note: scd_t, note_t, ...
		bool rest {false};
	};

	voice_t()=default;

	// Getters
	bar_t nbars() const;
	beat_t nbeats() const;
	int nevents() const;

	std::string print(const std::string& sep = " ") const;
private:
	ts_t ts_ {};
	std::vector<voice_t<T_note>::element_t> seq_ {};

};




