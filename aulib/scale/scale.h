#pragma once
#include "..\types\frq_t.h"
#include <vector>


// ref-frq, ntet, gint
std::vector<frq_t> frq_eqt(const std::vector<int>&,frq_t,int,int);

// scd (dn), ref-frq, ntet, gint
frq_t frq_eqt(int, frq_t,int,int);

// ref-frq, ntet, gint
double n_eqt(frq_t, frq_t,int,int);



//-----------------------------------------------------------------------------
// Class "scale"
// The purpose of a scale is to map the abstract note-representations scd_t, 
// ntstr_t, which carry no information about absolute pitch and which cannot
// be converted to sound, to a pitch value which can be sounded.  That is,
// a scale maps scd_t to and from ntstr_t and frq_t.  It also establishes 
// equivalence relations between pairs of abstract note elements for all such
// elements in its domain to create n equivalence classes.  Normally these 
// relations are based on pitch classes (ie, two notes differing in frq_t by 
// a factor of 2^m are members of the saem equivalence class), but there are 
// no restrictions on the specifics of the relations.  
//
//  A scale that is not completely insane probably wants to establish three
//  internal functions:  One to compute frq_t as a function of scd_t, an 
//  inverse that computes scd_t as a function of frq_t, and a third which 
//  computes ntl_t as a function of scd_t (probably by comparison to a static
//  list, taking advantage of the standard conversions between scd_t and 
//  rscdoctn_t).  
//
// A scale does ___ things:
// 1)  Establishes a constant, finite set of n note-letters ntl_t.  
// 2)  Maps any of {frq_t, scd_t, ntstr_t} to any of {frq_t, scd_t, 
//     ntstr_t, octn_t}.  
// 3)  
//

/*
class scale {
public:
	int n() const;
	std::string name() const;
	std::string description() const;
	bool isinsc(ntl_t) const;
	bool isinsc(frq_t) const;

	scd_t to_scd(ntstr_t) const;
	std::vector<scd_t> to_scd(std::vector<ntstr_t>) const;
	scd_t to_scd(frq_t) const;
	std::vector<scd_t> to_scd(std::vector<frq_t>) const;

	frq_t to_frq(ntstr_t) const;
	std::vector<frq_t> to_frq(std::vector<ntstr_t>) const;
	frq_t to_frq(scd_t) const;
	std::vector<frq_t> to_frq(std::vector<scd_t>) const;

	ntstr_t to_ntstr(scd_t) const;
	std::vector<ntstr_t> to_ntstr(std::vector<scd_t>) const;
	ntstr_t to_ntstr(frq_t) const;
	std::vector<ntstr_t> to_ntstr(std::vector<frq_t>) const;

	octn_t to_octn(scd_t) const;
	std::vector<octn_t> to_octn(std::vector<scd_t>) const;
	octn_t to_octn(frq_t) const;
	std::vector<octn_t> to_octn(std::vector<frq_t>) const;
private:
	int m_n {};
	std::string m_name {};
	std::string m_description {};

	std::vector<ntl_t> m_ntls;
	std::vector<frq_t> m_frq;
};
*/






