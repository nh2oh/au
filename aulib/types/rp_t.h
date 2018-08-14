#pragma once
#include "nv_t.h"
#include "beat_bar_t.h"
#include "ts_t.h"
#include <string>
#include <vector>

// Number of beats spanned by a given nv_t for a given ts.  
beat_t nbeat(ts_t const&, nv_t const&);

// Number of bars spanned by a given nv_t for a given ts.  
bar_t nbar(ts_t const&, nv_t const&);

// Print a vector of nv_ts
std::string printrp(ts_t const&, std::vector<nv_t> const&);

std::string rp_t_info();

// Convert a vector of durations (units ~ time) to an rp
//  delta_t, ts, bpm, resolution-in-seconds
std::vector<nv_t> deltat2rp(std::vector<double> const&, ts_t const&, 
	double const&, double const&);

//  rp, ts, bpm
std::vector<double> rp2deltat(std::vector<nv_t> const&, ts_t const&, 
	double const&);

double nv2dt(nv_t const&, ts_t const&, double const&);

std::string deltat2rp_demo();




