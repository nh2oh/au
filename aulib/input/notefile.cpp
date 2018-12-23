#include "notefile.h"
#include "..\util\au_util.h"
#include "..\util\au_error.h"
#include "dbklib\algs.h"
#include "..\util\au_algs_math.h"
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

bool notefileline::operator==(const notefileline& rhs) const {
	return (file_line_num == rhs.file_line_num &&
		aprx_eq(ontime,rhs.ontime) && 
		aprx_eq(offtime,rhs.offtime) &&
		aprx_eq(dt,rhs.dt) &&
		pitch == rhs.pitch);
}
bool nonnotefileline::operator==(const nonnotefileline& rhs) const {
	return (file_line_num == rhs.file_line_num &&
		linedata == rhs.linedata);
}
bool notefile::operator==(const notefile& rhs) const {
	return (fname == rhs.fname &&
		fpath == rhs.fpath &&
		nonnote_lines == rhs.nonnote_lines && 
		lines == rhs.lines &&
		error_lines == rhs.error_lines);
}

//
// Does not verify any sort of ordering (ex, that the events returned occur
// in order of increasing ontime).  Does not check for overlapping events
// (=> polyphony).  
//
// Lines conforming to: 
// "Note [ontime] [offtime] [pitch-num]"
// are written to .lines.  Lines not conforming to the above are written to
// .nonnote_lines.  This includes otherwise empty blank lines.  This ensures
// read_notefile() and write_notefile() complement eachother.  
//
// "Suspicious" lines:
// If  a given dt is <= 0, the index of the line in .lines is appended to
// .error_lines.  Note that nf.error_lines holds indices of the std::vector
// nf.lines, not physical line numbers in the file.  
//
//
notefile read_notefile(const std::string& filename) {
	auto fpath = std::filesystem::path(filename);

	auto f = std::ifstream(filename,std::ios::in);
	if (f.fail() || !f.is_open()) {
		notefile nf {};  nf.file_error = true;
		return nf;
	}

	std::vector<notefileline> nf_lines {};
	std::vector<nonnotefileline> nf_nnt_lines {};  // "non-note lines"
	std::vector<int> error_lines {};
	std::regex rx("Note\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)");
	std::string line {}; line.reserve(100);  // Expect ~25 chars/line
	
	int fline_num = 0;
	while (getline(f,line)) {
		++fline_num;
		auto o_matches = rx_match_captures(rx,line);
		if (!o_matches) {  // Is not a "Note ontime offtime pitch\n" line
			nonnotefileline curr_nnf_line {};
			curr_nnf_line.file_line_num = fline_num;
			curr_nnf_line.linedata = line;
			nf_nnt_lines.push_back(curr_nnf_line);
		} else {  // Is a "Note ontime offtime pitch\n" line
			auto matches = *o_matches;
		
			notefileline curr_nf_line;
			curr_nf_line.file_line_num = fline_num;
			curr_nf_line.ontime = std::stod(*(matches[1]));
			curr_nf_line.offtime = std::stod(*(matches[2]));
			curr_nf_line.dt = curr_nf_line.offtime - curr_nf_line.ontime;
			curr_nf_line.pitch = std::stoi(*(matches[3]));

			if (curr_nf_line.dt <= 0) {  // Checks for suspicious conditions...
				error_lines.push_back(static_cast<int>(nf_lines.size()));
			}

			nf_lines.push_back(curr_nf_line);
		}
	}  // to next line in file
	f.close();

	return notefile {false, fpath.filename().string(), fpath.parent_path().string(), 
		nf_nnt_lines, nf_lines, error_lines};
}

std::vector<std::chrono::milliseconds> notefile2dt(notefile const& nf) {
	std::vector<std::chrono::milliseconds> dt {}; dt.reserve(nf.lines.size());
	for (auto const& e : nf.lines) {
		dt.push_back(std::chrono::milliseconds(static_cast<int>(e.dt)));
	}
	return dt;
}

//
// Writes the notefile struct nf to the file indicated by nf.fpath, nf.fname.  If
// the file indicated does not exist, it is created; if the file indicated does
// exist, the contents are completely overwritten.  
//
// nf must contain entries in nf.lines and nf.nonnote_lines for every line to be
// written in the output file.  
//
// ontime and offtime have units of ms and are truncated to ints, even though 
// struct notefileline holds these values as doubles.  
//
bool write_notefile(const notefile& nf) {
	auto fpath = std::filesystem::path(nf.fpath + "/" + nf.fname);
	auto f = std::ofstream(fpath,std::ios_base::out);
	if (f.fail() || !f.is_open() || !f) {
		return false;
	}

	int curr_ln {0};  // curr _file_ line number, starts @ 1, not 0
	int i_lines {0}; int i_nntlines {0};
	bool nt_lines_finished {false}; bool nnt_lines_finished {false};
	while (curr_ln < (nf.lines.size() + nf.nonnote_lines.size())) {
		++curr_ln;
		nt_lines_finished = (i_lines >= nf.lines.size());
		nnt_lines_finished = (i_nntlines >= nf.nonnote_lines.size());

		if (!nt_lines_finished && !nnt_lines_finished) {
			// Check that there is not an entry for curr_ln in _both_ 
			// note_lines and nonnote_lines
			au_assert(nf.lines[i_lines].file_line_num != 
				nf.nonnote_lines[i_nntlines].file_line_num, "double what");
		}

		if (!nt_lines_finished && nf.lines[i_lines].file_line_num == curr_ln) {
			f << dbk::bsprintf("Note %6d %6d %6d\n", 
				nf.lines[i_lines].ontime, nf.lines[i_lines].offtime,
				nf.lines[i_lines].pitch);
			++i_lines;
		} else if (!nnt_lines_finished && nf.nonnote_lines[i_nntlines].file_line_num == curr_ln) {
			f << dbk::bsprintf("%s\n", nf.nonnote_lines[i_nntlines].linedata);
			++i_nntlines;
		} else {
			// curr_ln does not appear in either nf.lines or nf_nonnote_lines
			au_assert(false,"curr_ln does not appear in either nf.lines or nf_nonnote_lines");
			// Dying here ensures that at every loop iteration one of i_lines, i_nntlines
			// is incremented and that curr_ln actually refers to the phsyical line number 
			// in the output file.  
		}
	}  // To next line

	return true;
}


std::vector<ovl_idx> overlaps(const notefile& nf) {
	std::vector<ovl_idx> ovl {};  // "overlap"
	for (int i=0; i<nf.lines.size(); ++i) {
		for (int j=(i+1); j<nf.lines.size(); ++j) {
			bool on_t_overlaps = 
				(nf.lines[j].ontime > nf.lines[i].ontime &&  // j starts after i starts
				nf.lines[j].ontime < nf.lines[i].offtime);  // j starts before i ends
			bool off_t_overlaps = 
				(nf.lines[j].offtime > nf.lines[i].ontime &&  // j ends after i starts
				nf.lines[j].offtime < nf.lines[i].offtime);  // j ends before i ends

			if (on_t_overlaps && !off_t_overlaps) {  
				ovl.push_back({i,j,0});  // j starts within i but ends outside of i
			} else if (!on_t_overlaps && off_t_overlaps) {
				ovl.push_back({i,j,1});  // j ends within i but starts before i
			} else if (on_t_overlaps && off_t_overlaps) {
				ovl.push_back({i,j,2});  // j is completely contained within i
			}
		}
	}

	return ovl;
}



line_t<scd_t> notefile2line(const notefile& nf) {
	std::vector<scd_t> scds {};
	for (auto e : nf.lines) {
		//scds.push_back(scd_t{e.pitch});
		scds.push_back(e.pitch);
	}

	auto res = std::chrono::milliseconds(250);
	tempo_t tempo {150};
	ts_t ts {"4/4"};
	rp_t rp {ts,notefile2dt(nf),tempo,res};

	return line_t<scd_t> {scds,rp};
};

