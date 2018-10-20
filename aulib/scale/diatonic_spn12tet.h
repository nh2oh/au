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

class diatonic_spn12tet {
public:
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

	// Info
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
	void build_sc(spn12tet,ntl_t,mode);  // Delegated constructor
	scd_t scd_diatonic2spn12tet(scd_t) const;
	scd_t scd_spn12tet2diatonic(scd_t) const;

	std::string m_name {"Diatonic scale C"};
	std::string m_description {"whatever"};

	spn12tet m_sc_base {};  // C-chromatic 12-tet
	int m_n = 7;
	std::vector<int> m_ip {2,2,1,2,2,2,1};
	ntl_t m_ntl_base {"C"};
	int m_mode_idx {0};
	int m_shift_scd {0};  // the spn12tet scd that generates m_scale_ntl(0)

	std::vector<ntl_t> m_ntls {"C"_ntl,"D"_ntl,"E"_ntl,"F"_ntl,"G"_ntl,
		"A"_ntl,"B"_ntl};

};


