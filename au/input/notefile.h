#pragma once
#include <vector>
#include <string>

// Functions to read and write "notefiles" in "notelist" format:
// "Note [ontime] [offtime] [pitch-num]"
// Files may contain lines not beginning with "Note" that contain other data
// "Meta event", ...
//
// Pass notefileopts::pitchnum2spn as arg2 to subtract 12 from each of the
// pitch numbers.  Thus, convert 60 to 48 (middle C on the spn scale instead
// of C(5)).  
//
// In all the notefiles I have, ontime and offtime are always ints, but I am
// entering them as doubles so i can work with them more easily.  
//
struct notefile {
	double ontime {0};  // Milliseconds
	double offtime {0};
	int pitch {0}; // Middle C = 60
};

std::vector<notefile> read_notefile(std::string const&, int=0);

namespace notefileopts {
enum {
	pitchnum2spn = 1, // Subtract 12 from the [pitch] field
	someotheropt = 2,
};
};

