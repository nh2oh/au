#include "notefile.h"
#include "..\util\au_util.h"
#include <vector>
#include <string>
#include <fstream>
#include <regex>
#include <filesystem>
#include <chrono>

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
	auto fpath = std::filesystem::path(filename);

	auto f = std::ifstream(filename);
	if (!f.is_open()) {
		notefile nf {};  nf.file_error = true;
		return nf;
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
		curr_nf_line.file_line_num = fline_num;
		curr_nf_line.ontime = std::stod(*(matches[1]));
		curr_nf_line.offtime = std::stod(*(matches[2]));
		curr_nf_line.dt = curr_nf_line.offtime - curr_nf_line.ontime;
		curr_nf_line.pitch = std::stoi(*(matches[3]));

		if (curr_nf_line.dt <= 0) {  // Add checks for other suspicious conditions...
			error_lines.push_back(n);
		}

		nf_lines.push_back(curr_nf_line);
	}
	f.close();

	return notefile {false, fpath.filename().string(), fpath.relative_path().string(), 
		nf_lines, flags, error_lines};
}

std::vector<std::chrono::milliseconds> notefile2dt(notefile const& nf) {
	std::vector<std::chrono::milliseconds> dt {}; dt.reserve(nf.lines.size());
	for (auto const& e : nf.lines) {
		dt.push_back(std::chrono::milliseconds(static_cast<int>(e.dt)));
	}
	return dt;
}




