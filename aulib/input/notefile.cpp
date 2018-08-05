#include "notefile.h"
#include "..\util\au_util_all.h"
#include <vector>
#include <string>
#include <fstream>
#include <regex>

//
// Does not verify any sort of ordering (ex, that the events returned occur
// in order of increasing ontime).  
//
// Will not capture (-) values for any elements of struct notefile
//
// Lines not conforming to: "Note [ontime] [offtime] [pitch-num]" are
// skipped (ignored).
//
//
//

notefile read_notefile(std::string const& filename, int const& flags) {
	auto f = std::ifstream(filename);
	au_assert(f.is_open(),"Could not open file");

	int scale_fctr = 1;
	if (flags & notefileopts::seconds) {
		scale_fctr = 1000;
	}

	std::vector<notefileline> nf_lines; nf_lines.reserve(100);
	std::vector<int> error_lines {};
	std::regex rx("Note\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)");
	std::string line {}; line.reserve(100);  // Expect ~25 chars/line
	int fline_num = 0; int n = 0;
	while (getline(f,line)) {
		++fline_num;
		auto o_matches = rx_match_captures(rx,line);
		if (!o_matches) { continue; }
		auto matches = *o_matches;

		++n;
		notefileline curr_nf_line;
		curr_nf_line.file_line_num = fline_num/scale_fctr;
		curr_nf_line.ontime = std::stod(*(matches[1]))/scale_fctr;
		curr_nf_line.offtime = std::stod(*(matches[2]))/scale_fctr;
		curr_nf_line.dt = curr_nf_line.offtime - curr_nf_line.ontime;
		curr_nf_line.pitch = std::stoi(*(matches[3]));

		if (curr_nf_line.dt <= 0) {  // Add checks for other suspicious conditions...
			error_lines.push_back(n);
		}

		nf_lines.push_back(curr_nf_line);
	}
	f.close();
	au_assert(nf_lines.size() > 0, "Didn't get any lines!");

	return notefile {filename, nf_lines, flags, error_lines};
}

/*
double notefileelement2dt(notefile const& nf, int const& flags) {
	double dt {nf.offtime-nf.ontime};
	if (flags & notefileopts::seconds) {
		dt /= 1000;
	}
	return dt;
}
*/

std::vector<double> notefile2dt(notefile const& nf) {
	std::vector<double> dt {}; dt.reserve(nf.lines.size());
	for (auto const& e : nf.lines) {
		dt.push_back(e.dt);
	}
	return dt;
}




