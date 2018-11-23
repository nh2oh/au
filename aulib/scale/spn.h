#pragma once
#include "scale.h"
#include "..\types\frq_t.h"
#include "..\types\ntl_t.h"
#include "..\types\scale_iterator.h"
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
class spn {
public:
	using iterator = typename scale_const_iterator<spn>;
	// Constructors
	spn()=default;  // Generates A440 ("A(4)" == 440 Hz)
	explicit spn(pitch_std);

	// Getters
	std::string name() const;
	std::string description() const;
	std::string print() const;
	std::string print(int,int) const;

	bool isinsc(const ntl_t&) const;
	bool isinsc(const frq_t&) const;
	bool isinsc(const note_t&) const;

	int to_scd(const ntl_t&, const octn_t&) const;
	int to_scd(const note_t&) const;
	int to_scd(const frq_t&) const;
	std::vector<int> to_scd(const std::vector<note_t>&) const;

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

	// Methods
	base_ntl_idx_t base_ntl_idx(const ntl_t&, const octn_t&) const;
	base_ntl_idx_t base_ntl_idx(const frq_t&) const;
	
	// Data
	pitch_std m_pstd {};
	int N {12};
	int m_shift_scd {57};  // the scd that generates the ref frq; 57 => A(4)
	std::vector<ntl_t> m_ntls {"C"_ntl,"C#"_ntl,"D"_ntl,"D#"_ntl,
		"E"_ntl,"F"_ntl,"F#"_ntl,"G"_ntl,"G#"_ntl,"A"_ntl,"A#"_ntl,"B"_ntl};
	std::string m_name {"12-tone chromatic"};
	std::string m_description {"12-tone equal-tempered with A(4)=440Hz; SPN"};
};

