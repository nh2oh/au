#include "notefile.h"
#include "..\util\au_util.h"
#include "..\types\line_t.h" // for notefile2line()
#include "..\types\scd_t.h" // for notefile2line()
#include "..\types\ts_t.h" // for notefile2line()
#include "..\types\rp_t.h" // for notefile2line()
#include "..\types\beat_bar_t.h"  // tempo_t for notefile2line()
#include <vector>
#include <string>
#include <fstream>
#include <regex>
#include <filesystem>
#include <chrono>

//
// Does not verify any sort of ordering (ex, that the events returned occur
// in order of increasing ontime).  Does not check for overlapping events
// (=> polyphony).  Lines not conforming to: 
// "Note [ontime] [offtime] [pitch-num]" are skipped (ignored).
//
// If dt is <= 0, the line is still added to the nf.lines vector, but the 
// line  number is recorded in .error_lines.  Note that nf.error_lines 
// => indices of nf.lines, not physical line numbers in the file.  It is
// always possible to determine the file line number for a given 
// nf.error_lines[i] as nf.lines[nf.error_lines[i]].file_line_num.  
//
//
//
//

notefile read_notefile(const std::string& filename, int const& flags) {
	auto fpath = std::filesystem::path(filename);

	auto f = std::ifstream(filename);
	if (!f.is_open()) {
		notefile nf {};  nf.file_error = true;
		return nf;
	}

	std::vector<notefileline> nf_lines {};
	std::vector<int> error_lines {};
	std::regex rx("Note\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)");
	std::string line {}; line.reserve(100);  // Expect ~25 chars/line
	
	int fline_num = 0;
	int n = 0;  // The idx of .lines presently being written
	while (getline(f,line)) {
		++fline_num;
		auto o_matches = rx_match_captures(rx,line);
		if (!o_matches) { continue; }
		auto matches = *o_matches;
		
		notefileline curr_nf_line;
		curr_nf_line.file_line_num = fline_num;
		curr_nf_line.ontime = std::stod(*(matches[1]));
		curr_nf_line.offtime = std::stod(*(matches[2]));
		curr_nf_line.dt = curr_nf_line.offtime - curr_nf_line.ontime;
		curr_nf_line.pitch = std::stoi(*(matches[3]));

		if (curr_nf_line.dt <= 0) {  // Checks for suspicious conditions...
			error_lines.push_back(n);
		}

		nf_lines.push_back(curr_nf_line);
		++n;
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


bool write_notefile(const notefile& nf) {
	auto fpath = std::filesystem::path(nf.fpath + nf.fname);
	auto f = std::ofstream(fpath);
	if (!f.is_open()) {
		return false;
	}

	for (const auto& e : nf.lines) {
		//...
	}

	return true;
}


line_t<scd_t> notefile2line(const notefile& nf) {
	std::vector<scd_t> scds {};
	for (auto e : nf.lines) {
		scds.push_back(scd_t{e.pitch});
	}

	auto res = std::chrono::milliseconds(250);
	tempo_t tempo {150};
	ts_t ts {"4/4"};
	rp_t rp {ts,notefile2dt(nf),tempo,res};

	return line_t<scd_t> {rp,scds};
};

