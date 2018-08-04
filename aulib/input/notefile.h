#pragma once
#include <vector>
#include <string>

//
// Functions to read and write "notefiles" in "notelist" format:
// "Note [ontime] [offtime] [pitch-num]"
// Files may contain lines not beginning with "Note" that contain other data
// "Meta event", ...
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

double notefileelement2dt(notefile const&, int const& = 0);
std::vector<double> notefile2dt(std::vector<notefile> const&, int const& = 0);

namespace notefileopts {
enum {
	seconds = 1,
	someotheropt = 2,
};
};

