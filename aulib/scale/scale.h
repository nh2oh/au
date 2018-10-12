#pragma once
#include "..\types\types_all.h"
#include <string>
#include <vector>

// Forward declarations from ntl_t.h
/*class ntl_t;
class frq_t;
class scd_t;
class ntstr_t;
class ntl_t;
class rscdoctn_t;
*///struct rscdoct_t;

//-----------------------------------------------------------------------------
// Class "scale"
// The purpose of a scale is to map the abstract note-representations scd_t, 
// ntstr_t, which carry no information about absolute pitch and which cannot
// be converted to sound, to a pitch value which can be sounded.  That is,
// a scale maps scd_t and ntstr_t to frq_t.  It also establishes the mapping 
// between scd_t and rscdoctn_t by defining n, the number of scd_t's per 
// octave.  
// A scale does ___ things:
// 1)  It establishes a modular arithmetic on elements of type scd_t which
//     associates an scd_t with a pair {rscd, oct}.  Although the relation
//     is "established" by the scale (since the scale sets m_n), the 
//     definition of the relationship between scd_t and rscdoctn_t, that is,
//     {scd, sc.m_n} <---> {rscd, oct, sc.m_n} is general and the scale 
//     can't muck with it.  See scd_t.h.  
// 2)  It defines a set of m_n unique ntl_t's.  
// 3)  It maps, on demand, any of {frq_t, scd_t, ntstr_t} to any of {frq_t, 
//     scd_t, ntstr_t, octn_t}.  
//

/*
class scale {
public:
	int n() const;
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
};
*/








//  The scale is not required to make any guarantee's concerning its
//  mappings.  In particular, it need not be static: the frq_t returned
//  for a particular scd may change with time.  For this reason,
//  even though the user can compute equivalent pairs scd_t, rscdoctn_t, 
//  the scale may not return the same result to successive queries with such 
//  equivalent pairs.  HOWEVER, a scale is not allowed to define its own
//  conversion between scd_t and rscdoctn_t: Two simultanious queries
//  to a "dynamic" scale with equivalent inputs must return the same value.  
// 
//  scd_t        ----------> ntstr_t, frq_t 
//  frq_t        ----------> scd_t, ntstr_t
//  ntstr_t      ----------> scd_t, frq_t
//  ... also octn_t ...
// 
//  The functions a scale must export:
//  to_ntstr(scd,frq_t) -> {ntl,oct}
//  to_scd(frq_t,{ntl,oct}) -> scd_t	// note: no scd_t input
//  to_octn(frq_t,ntstr_t,scd_t) -> {rscd,oct}  // note: no {rscd,oct} input
//  to_frq(scd,{rscd,oct},{ntl,oct}) -> frq_t
//  isinsc(frq_t) -> true|false
//  isinsc({ntl,oct}) -> true|false
//  get_ntls()  // return all ntl_t's
//  m_n()  // So the user can feed rscd2scd(), scd2rscd()
//
//  A scale that is not completely insane probably wants to establish three
//  internal functions:  One to compute frq_t as a function of scd_t, an 
//  inverse that computes scd_t as a function of frq_t, and a third which 
//  computes ntl_t as a function of scd_t (probably by comparison to a static
//  list, taking advantage of the standard conversions between scd_t and 
//  rscdoctn_t).  
//
//
//
//
//


/*

class scale {
public:
	//----------------------------------------------------------------------------
	// Constructors
	// There is no "default" zero-argument constructor!

	// Name, description, npc table; for the rare (?) case where
	// the caller has a valid npc_table_t handy.  
	// Seperate std::vector of std::vector of ntstrs and std::vector of 
	// frqs; basically just wraps the appropriate overload of coerce2npc()
	scale(std::string, std::string, std::vector<std::vector<ntl_t>>, 
		std::vector<frq_t>);

	//----------------------------------------------------------------------------
	// Functions for user-modification post-construction
	bool set_name(const std::string&);
	bool set_description(const std::string&);
	// overwrite existing npc_table line (frq and _all_ ntstrs)
	bool set_npc(const scd_t&, const frq_t&, const std::vector<ntl_t>&);
	// overwrite existing frq entry
	bool set_frq(const scd_t&, const frq_t&);
	// overwrite existing ntstr entry (_all_ ntstrs)
	bool set_ntls(const scd_t&, const std::vector<ntl_t>&);
	// insert _before_ scd_t argument
	bool insert_npc(const scd_t&, const frq_t&, const std::vector<ntl_t>&);
	// Delete row in npc table
	bool delete_npc(const scd_t&);

	//----------------------------------------------------------------------------
	// Functions for extracting data from the scale

	std::string get_name() const;
	std::string get_description() const;

	// These functions simply dump _all_ of the data in the npc_table.  No
	// facility is provided to allow the user to pick and choose scd's, frqs, etc
	std::vector<std::vector<ntl_t>> get_ntls() const;
	std::vector<frq_t> get_frqs() const;

	// Get a single ntl/ntstr, frq, or scd for the input.  If more than one 
	// element of the npc "table" matches the input, return only the first one.  
	// These functions do not implement scd->rscd logic.  When this is needed, 
	// they call scd2rscd() or rscd2scd().  I do not want to put this 
	// functionality into the scale member functions since access to internal 
	// scale datastructures is not needed to make the conversion.  
	ntstr_t get_ntstr(frq_t const&) const;
	ntstr_t get_ntstr(scd_t const&) const;

	ntl_t get_ntl(frq_t const&) const;
	ntl_t get_ntl(scd_t const&) const;

	frq_t get_frq(scd_t const&) const;
	frq_t get_frq(ntstr_t const&) const;

	scd_t get_scd(ntstr_t const&) const;
	scd_t get_scd(frq_t const&) const;

	rscdoctn_t get_rscdoct(frq_t const&) const;
	scd_t get_rscd(ntl_t const&) const;

	// size of the npc table
	int n() const;

	// "single" std::vector<ntl_t> or frq_t as a function of scd
	bool isinsc(ntstr_t const&) const;
	bool isinsc(ntl_t const&) const;
	bool isinsc(frq_t const&) const;

	// Pretty-print
	std::string print(std::string mode = "default") const;
private:
	//----------------------------------------------------------------------------
	// Data members
	std::string m_name;
	std::string m_description;
	std::vector<std::vector<ntl_t>> m_ntls;
	std::vector<frq_t> m_frqs;
	int m_n;

	//----------------------------------------------------------------------------
	// Intermediate types

	//----------------------------------------------------------------------------
	// Functions
	//scd_t rscd2scd(scd_t const&) const;
	//scd_t scd2rscd(scd_t const&) const;
	bool validate();
};

*/