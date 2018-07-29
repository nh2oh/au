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

std::vector<notefile> read_notefile(std::string const& filename, int flags) {
	auto f = std::ifstream(filename);
	au_assert(f.is_open(),"Could not open file");

	std::vector<notefile> result; result.reserve(100);
	std::regex rx("Note\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)");
	std::string line {}; line.reserve(100);  // Expect ~25 chars/line
	while (getline(f,line)) {
		auto o_matches = rx_match_captures(rx,line);
		if (!o_matches) { continue; }
		auto matches = *o_matches;

		result.push_back({std::stod(*(matches[1])),std::stod(*(matches[2])),
			std::stoi(*(matches[3]))});

		if (flags & notefileopts::pitchnum2spn) {
			result.back().pitch -= 12;
		}
	}
	f.close();
	au_assert(result.size() > 0, "Didn't get any lines!");

	return result;
}







