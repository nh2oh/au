#pragma once
#include <string>
#include <vector>

class note_value;
class ts_t;
class bar_t;
class beat_t;

// Number of beats spanned by a given note_value for a given ts.  
beat_t nbeat(ts_t const&, note_value const&);

// Number of bars spanned by a given note_value for a given ts.  
bar_t nbar(ts_t const&, note_value const&);

// Print a vector of note_values
std::string printrp(ts_t const&, std::vector<note_value> const&);

std::string rp_t_info();

// Convert a vector of durations (units ~ time) to an rp
//  delta_t, ts, bpm, resolution-in-seconds
std::vector<note_value> deltat2rp(std::vector<double> const&, ts_t const&, 
	double const&, double const&);

//  rp, ts, bpm
std::vector<double> rp2deltat(std::vector<note_value> const&, ts_t const&, 
	double const&);

std::string deltat2rp_demo();




