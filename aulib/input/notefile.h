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
namespace notefileopts {
enum {
	seconds = 1,
	someotheropt = 2,
};
};

struct notefileline {
	int file_line_num {0};  // The actual line number in the txt file
	double ontime {0.0};  // Milliseconds by default
	double offtime {0.0};
	double dt {0.0};
	int pitch {0}; // Middle C = 60
};

struct notefile {
	std::string fname {};
	std::vector<notefileline> lines {};
	int opts {0};
	std::vector<int> error_lines {};
		// idxs of notefile.lines where dt <= 0, etc
};


notefile read_notefile(std::string const&, int const& = 0);

//double notefileelement2dt(notefile const&, int const& = notefileopts::defaultopts);
std::vector<double> notefile2dt(notefile const&);



