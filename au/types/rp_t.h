#pragma once
//#include "beat_bar_t.h"
//#include "ts_t.h"
#include <string>
#include <vector>

class note_value;
class ts_t;
class bar_t;
class beat_t;





//-----------------------------------------------------------------------------
// non-ts_t class functions
//
beat_t nbeat(ts_t const&, note_value const&);
bar_t nbar(ts_t const&, note_value const&);
std::string printrp(ts_t const&, std::vector<note_value> const&);

std::string rp_t_info();

std::vector<std::vector<int>> tmetg(ts_t, std::vector<note_value>, std::vector<beat_t>);  // ts, dp, phase
//std::string print_tg(std::vector<std::vector<int>> tg);

// Convert a vector of durations (units ~ time) to an rp
//  delta_t, ts, bpm, resolution-in-seconds
std::vector<note_value> deltat2rp(std::vector<double> const&, ts_t const&, 
	double const&, double const&);

//  rp, ts, bpm
std::vector<double> rp2deltat(std::vector<note_value> const&, ts_t const&, 
	double const&);



std::string deltat2rp_demo();




