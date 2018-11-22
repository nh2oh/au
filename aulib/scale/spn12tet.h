#pragma once
#include "scale.h"
#include "..\types\frq_t.h"
#include "..\types\ntl_t.h"
#include <vector>
#include <string>

//
// "spn" is a standard that associates ntl_t's w/ octn_t's, and to each ntl_t, 
// octn_t pair, associates a numeric scale degree scd_t.  It specifies how pitch
// _names_ should be notated (in particular, at which note letter pair the 
// octave boundry should occur); it does not stipulate frequencies for the pitches
// of the note letters/scale-degrees.  
// In SPN, there are 12 note letters A,A#,B,C,...,G,G#.  The note C is the 
// first note of each octave, hence the zeroth scale degree, 
// scd_t == 0 <=> C(0)
// In consequence, scd_t == 48 => C(4) => "Middle C"
//
// The default constructor uses the A440 pitch standard: A(4) == 440 Hz.  
// Changing the pitch standard does _not_ change the ordering of the ntl's
// nor the location of the octave breaks.  See the note above.  
//
// TODO:  Should pitch-std be a template param for the scale ?
// 
//
class spn12tet {
public:
	class scd3_t {
	public:
		// Note these ctors are public... in general scale scd3_t's need not provide
		// public ctors...
		scd3_t()=default;
		explicit scd3_t(int,const spn12tet*);

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
		const spn12tet *m_sc {};
	};
	

	// Constructors
	/*explicit*/ spn12tet()=default;  // Generates A440 ("A(4)" == 440 Hz)
	explicit spn12tet(pitch_std);

	// Getters
	std::string name() const;
	std::string description() const;
	std::string print() const;
	std::string print(int,int) const;

	bool isinsc(const ntl_t&) const;
	bool isinsc(const frq_t&) const;
	bool isinsc(const note_t&) const;
	
	spn12tet::scd3_t to_scd(const ntl_t&, const octn_t&) const;
	spn12tet::scd3_t to_scd(const note_t&) const;
	spn12tet::scd3_t to_scd(const frq_t&) const;
	spn12tet::scd3_t to_scd(const int&) const;
		// to_scd(0) is analagous to getting a vector iterator by calling .front()
		// to_scd(5) is alalagous to auto it = myvec.front()+5;
	std::vector<spn12tet::scd3_t> to_scd(const std::vector<note_t>&) const;
private:
	struct base_ntl_idx_t {
		bool is_valid {false};
		int ntl_idx {0};  // [0,m_ntls.size())
		int scd_idx {0};

		// Expect:  ntl_idx==(scd_idx+m_ntls.size())%(m_ntls.size);
	};

	// Methods
	base_ntl_idx_t base_ntl_idx(const ntl_t&, const octn_t&) const;
	base_ntl_idx_t base_ntl_idx(const frq_t&) const;

	note_t to_note(int) const;  // Getter called by scd3_t::operator*()
	
	// Data
	pitch_std m_pstd {};
	/*const*/ int N {12};
	int m_shift_scd {57};  // the scd that generates the ref frq; 57 => A(4)
	/*const*/ std::vector<ntl_t> m_ntls {"C"_ntl,"C#"_ntl,"D"_ntl,"D#"_ntl,
		"E"_ntl,"F"_ntl,"F#"_ntl,"G"_ntl,"G#"_ntl,"A"_ntl,"A#"_ntl,"B"_ntl};
	std::string m_name {"12-tone chromatic"};
	std::string m_description {"12-tone equal-tempered with A(4)=440Hz; SPN"};
};

