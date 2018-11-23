#pragma once
#include "..\types\cent_oct_t.h"
#include "..\types\frq_t.h"
#include "..\types\ntl_t.h"
#include "..\types\scale_iterator.h"
#include "spn.h"
#include <vector>
#include <string>

//
// As the name "_spn" implies, octave breaks occur & are named according to the 
// "spn" standard (see scale::spn), that is, between B and C.  It does not matter
// what the user has chosen as the root note or mode for the scale.  The zeroth scale
// degree is the zeroth-octave of the user-selected root note on the scale::spn 
// scale.  Since the ctors only accept ntl_t arguments, it is not possible to set 
// the zeroth scd to an spn-note w an octave > 0, but this feature could easily be 
// added in the future.  
//
//

class diatonic_spn {
public:
	using iterator = typename scale_const_iterator<diatonic_spn>;

	class scd3_t {
	public:
		// Note these ctors are public... in general scale scd3_t's need not provide
		// public ctors...
		explicit scd3_t(int,const diatonic_spn*);

		note_t operator*() const;
		scd3_t& operator++();  // prefix
		scd3_t operator++(int);  // postfix
		scd3_t& operator--();  // prefix
		scd3_t operator--(int);  // postfix
		scd3_t& operator-=(const scd3_t&);
		scd3_t& operator-=(const int&);
		scd3_t& operator+=(const int&);
		friend int operator-(const scd3_t&,const scd3_t&);
		//bool operator==(const scd3_t&) const;
		//bool operator>(const scd3_t&) const;
		//bool operator<(const scd3_t&) const;
	private:
		int m_val {0};
		const diatonic_spn *m_sc {};
	};
	enum mode {
		major = 0,  // W-W-H-W-W-W-H 
		minor = 5,  // W-H-W-W-H-W-W 

		ionian = 0,  // W-W-H-W-W-W-H 
		dorian = 1,  // W-H-W-W-W-H-W
		phygrian = 2,  // H-W-W-W-H-W-W
		lydian = 3,  // W-W-W-H-W-W-H 
		mixolydian = 4,  // W-W-H-W-W-H-W
		aeolian = 5,  // W-H-W-W-H-W-W 
		locrian = 6  // H-W-W-H-W-W-W
	};

	// Constructors -- all delegate to build_sc()
	explicit diatonic_spn();
	explicit diatonic_spn(ntl_t,mode);
	explicit diatonic_spn(spn,ntl_t,mode);

	// Getters
	std::string name() const;
	std::string description() const;
	std::string print() const;
	std::string print(int,int) const;

	bool isinsc(const ntl_t&) const;
	bool isinsc(const frq_t&) const;
	bool isinsc(const note_t&) const;
	
	diatonic_spn::scd3_t to_scd(const note_t&) const;
	diatonic_spn::scd3_t to_scd(const ntl_t&, const octn_t&) const;
	diatonic_spn::scd3_t to_scd(const frq_t&) const;
	diatonic_spn::scd3_t to_scd(const int&) const;
	std::vector<diatonic_spn::scd3_t> to_scd(const std::vector<note_t>&) const;
	std::vector<diatonic_spn::scd3_t> to_scd(const std::vector<frq_t>&) const;

	note_t operator[](int) const;
	iterator zero() const;
private:
	struct base_ntl_idx_t {
		bool is_valid {false};
		int ntl_idx {0};  // [0,m_ntls.size())
		int scd_idx {0};
	};
	base_ntl_idx_t base_ntl_idx(const ntl_t&, const octn_t&) const;
	base_ntl_idx_t base_ntl_idx(const frq_t&) const;

	void build_sc(spn,ntl_t,mode);  // Delegated constructor

	note_t to_note(int) const;  // Getter called by scd3_t::operator*()

	std::string m_name {"Diatonic scale"};
	std::string m_description {"whatever"};

	spn m_sc_base {};  // C-chromatic 12-tet
	int m_n = 7;
	std::vector<int> m_ip {2,2,1,2,2,2,1};  // Chromatic steps between adjacent scds
	int m_mode_idx {0};

	std::vector<ntl_t> m_ntls {};
};

