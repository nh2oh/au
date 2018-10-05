#pragma once
#include <vector>
#include <string>
#include <chrono>

//
// Functions to read and write "notefiles" in "notelist" format:
// "Note [ontime] [offtime] [pitch-num]"
//
// TODO:  Function to examine a t vector and pull out overlapping events
//   into multiple "voices."  This can not be done w/a dt vector b/c 
//   absolute temporal positions have been tossed.  
//


struct notefileline {
	notefileline() = default;
	int file_line_num {0};  // The actual line number in the txt file
	double ontime {0.0};  // Milliseconds by default
	double offtime {0.0};
	double dt {0.0};
	int pitch {0}; // Middle C = 60
};

struct nonnotefileline {
	int file_line_num {0};  // line num in the txt file; starts @ 1, not 0
	std::string linedata {};
};

struct notefile {
	notefile() = default;
	bool file_error {true};
		// Indicates some error reading or opening the file.  Does not
		// depend on the contents of the file. To see if the file contents
		// are in some way suspicious, see member error_lines.  

	std::string fname {};
	std::string fpath {};
	std::vector<nonnotefileline> nonnote_lines {};
	std::vector<notefileline> lines {};
	//int opts {0};
	std::vector<int> error_lines {};
		// idxs of notefile.lines where dt <= 0, etc.  Idxs to the vector, not
		// file line numbers.  
};

notefile read_notefile(const std::string&);
bool write_notefile(const notefile&);
std::vector<std::chrono::milliseconds> notefile2dt(notefile const&);

struct ovl_idx {
	int idxa {};
	int idxb {};
	int type {};
};
std::vector<ovl_idx> overlaps(const notefile&);


// Attempt at forward declarations...
class scd_t;
template<typename T> class line_t;

line_t<scd_t> notefile2line(const notefile&);


