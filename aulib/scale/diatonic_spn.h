#pragma once
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
	
	int to_scd(const note_t&) const;
	int to_scd(const ntl_t&, const octn_t&) const;
	int to_scd(const frq_t&) const;
	std::vector<int> to_scd(const std::vector<note_t>&) const;
	std::vector<int> to_scd(const std::vector<frq_t>&) const;

	template<typename T>
	note_t to_note(const T& query) const {
		int scd = this->to_scd(query);
		return this->operator[](scd);
	};

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

	std::string m_name {"Diatonic scale"};
	std::string m_description {"whatever"};
	spn m_sc_base {};  // C-chromatic 12-tet
	int m_n = 7;  // TODO:  const
	std::vector<int> m_ip {2,2,1,2,2,2,1};  // Chromatic steps between adjacent scds
		// TODO:  const
	int m_mode_idx {0};  // TODO:  const
	std::vector<ntl_t> m_ntls {};    // TODO:  const??
};

