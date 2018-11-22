#pragma once
#include "..\types\cent_oct_t.h"
#include "..\types\frq_t.h"
#include "..\types\ntl_t.h"
#include "..\types\scd_t.h"
#include "spn12tet.h"
#include <vector>
#include <string>

//
// As the name _spn12tet implies, octave breaks occur & are named 
// according to the "spn" standard, that is, between B and C.  It 
// does not matter what the user has chosen as the root note for the
// scale.  In the future, will create a more generalized scale type
// that allows this to be customized.  
// The zeroth scale degree is the zeroth-octave of the user-selected 
// root note, where the frq_t of this is determined by spn12tet.  
//
//
//

// TODO:  Base on spn12tet3; new name == spn_diatonic ?

class diatonic_spn12tet {
public:
	class scd3_t {
	public:
		// Note these ctors are public... in general scale scd3_t's need not provide
		// public ctors...
		explicit scd3_t(int,const diatonic_spn12tet*);

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
		const diatonic_spn12tet *m_sc {};
	};
	enum mode {
		major = 0,
		minor = 5,

		ionian = 0,
		dorian = 1,
		phygrian = 2,
		lydian = 3,
		mixolydian = 4,
		aeolian = 5,
		locrian = 6
	};

	// Constructors -- all delegate to build_sc()
	diatonic_spn12tet()=default;
	explicit diatonic_spn12tet(ntl_t,mode);
	explicit diatonic_spn12tet(spn12tet,ntl_t,mode);

	// Getters
	std::string name() const;
	std::string description() const;
	std::string print() const;
	std::string print(int,int) const;

	bool isinsc(const ntl_t&) const;
	bool isinsc(const frq_t&) const;
	bool isinsc(const note_t&) const;
	
	diatonic_spn12tet::scd3_t to_scd(const note_t&) const;
	diatonic_spn12tet::scd3_t to_scd(const ntl_t&, const octn_t&) const;
	diatonic_spn12tet::scd3_t to_scd(const frq_t&) const;
	diatonic_spn12tet::scd3_t to_scd(const int&) const;
		// to_scd(0) is analagous to getting a vector iterator by calling .front()
		// to_scd(5) is alalagous to auto it = myvec.front()+5;
	std::vector<diatonic_spn12tet::scd3_t> to_scd(const std::vector<note_t>&) const;
	std::vector<diatonic_spn12tet::scd3_t> to_scd(const std::vector<frq_t>&) const;


private:
	struct base_ntl_idx_t {
		bool is_valid {false};
		int ntl_idx {0};  // [0,m_ntls.size())
		int scd_idx {0};
	};
	base_ntl_idx_t base_ntl_idx(const ntl_t&, const octn_t&) const;
	base_ntl_idx_t base_ntl_idx(const frq_t&) const;

	void build_sc(spn12tet,ntl_t,mode);  // Delegated constructor

	note_t to_note(int) const;  // Getter called by scd3_t::operator*()

	std::string m_name {"Diatonic scale C"};
	std::string m_description {"whatever"};

	spn12tet m_sc_base {};  // C-chromatic 12-tet
	int m_n = 7;
	std::vector<int> m_ip {2,2,1,2,2,2,1};
	ntl_t m_ntl_base {"C"};
	int m_mode_idx {0};
	int m_shift_basentl {0};  // the spn12tet scd that generates m_scale_ntl(0)

	std::vector<ntl_t> m_ntls {"C"_ntl,"D"_ntl,"E"_ntl,"F"_ntl,"G"_ntl,
		"A"_ntl,"B"_ntl};

};

