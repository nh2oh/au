#pragma once
#include "scale.h";
#include "..\types\cent_oct_t.h"
#include "..\types\frq_t.h"
#include "..\types\ntl_t.h"
#include "..\types\scd_t.h"
#include <vector>
#include <string>

//
// "spn" is a standard that associates ntl_t's w/ octn_t's.  That is, it
// specifies how pitch names should be notated (in particular, at which 
// note letter pair the octave boundry should occur); it does not stipulate
// frequencies for the pitches of the note letters.  
// In SPN, there are 12 note letters A,A#,B,C,...,G,G#.  The note C is the 
// first note of each octave, hence the zeroth scale degree, 
// scd_t == 0 <=> C(0)
//
// The default constructor uses the A440 pitch standard: A(4) == 440 Hz.  
// Changing the pitch standard does _not_ change the ordering of the ntl's
// nor the location of the octave breaks.  See the note above.  
//

class spn12tet {
public:
	struct pitch_std {
		pitch_std()=default;
		// These control the pitch standard, ie, the actual frq values of the 
		// named pitches of SPN (C(i),C#(i),D(i),...B(i),C(i+1),...).  
		// Changing ref_ntstr here does _not_ change the note @ which the 
		// octave boundry occurs.  
		ntstr_t ref_ntstr {"A"_ntl,4};
		frq_t ref_frq {440};
		int gen_int {2};
		int ntet {12};
	};

	// Constructors
	spn12tet()=default;  // Generates A440 ("A(4)" == 440 Hz)
	spn12tet(spn12tet::pitch_std);

	int n() const;
	std::string name() const;
	std::string description() const;
	bool isinsc(ntl_t) const;
	bool isinsc(frq_t) const;
	std::string print(scd_t,scd_t) const;

	scd_t to_scd(ntstr_t) const;
	std::vector<scd_t> to_scd(const std::vector<ntstr_t>&) const;
	scd_t to_scd(frq_t) const;
	std::vector<scd_t> to_scd(const std::vector<frq_t>&) const;

	frq_t to_frq(ntstr_t) const;
	std::vector<frq_t> to_frq(const std::vector<ntstr_t>&) const;
	frq_t to_frq(scd_t) const;
	std::vector<frq_t> to_frq(const std::vector<scd_t>&) const;

	ntstr_t to_ntstr(scd_t) const;
	std::vector<ntstr_t> to_ntstr(const std::vector<scd_t>&) const;
	ntstr_t to_ntstr(frq_t) const;
	std::vector<ntstr_t> to_ntstr(const std::vector<frq_t>&) const;

	octn_t to_octn(scd_t) const;
	//std::vector<octn_t> to_octn(const std::vector<scd_t>&) const;
	octn_t to_octn(frq_t) const;
	//std::vector<octn_t> to_octn(const std::vector<frq_t>&) const;

private:
	pitch_std m_ps {};
	int m_shift_scd {57};  // the scd that generates the ref frq; 57 => A(4)
	std::vector<ntl_t> m_ntls {"C"_ntl,"C#"_ntl,"D"_ntl,"D#"_ntl,
		"E"_ntl,"F"_ntl,"F#"_ntl,"G"_ntl,"G#"_ntl,"A"_ntl,"A#"_ntl,"B"_ntl};

	// Info for name(), description()
	std::string m_name {"12-tone chromatic"};
	std::string m_description {"12-tone equal-tempered with A(4)=440Hz; SPN"};

};

