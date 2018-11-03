#pragma once
#include "scale.h"
#include "..\types\scd3_t.h"
#include "..\types\cent_oct_t.h"
#include "..\types\frq_t.h"
//#include "..\types\ntl_t.h"   // #include's scd_t.h
#include <vector>
#include <string>
#include <array>

//
// In this alternative design...
// - scd_t defines its own conversion to octaves/rscd's
//
//
//
class spn12tet3 {
public:
	enum { N = 12 };  // cf BS C++ FAQ: "How do i define an in-class constant?"
	using scd_t = scd3_t<spn12tet3::N>;
	static bool is_ntl(const std::string&);  // PT to spn12tet3::ntl3_t::is_ntl()

	//static bool ntl_eq(const std::string& lhs, const std::string& rhs);
	//static bool ntl_lt(const std::string& lhs, const std::string& rhs);

	class ntl3_t {
	public:
		explicit ntl3_t()=default;
		explicit ntl3_t(const std::string&);
		explicit ntl3_t(const std::string&, int);  // ntl, octave-number
		explicit ntl3_t(const char*, int);  // ntl, octave-number

		std::string print() const;

		static bool is_ntl(const std::string&);
		bool operator==(const ntl3_t&) const;
		bool operator<(const ntl3_t&) const;
	private:
		static const std::array<char,7> base_ntls;
		static const std::array<char,2> ntl_modifiers;  // Todo: Should get rid of this crap
		//std::string m_ntl {"C"};
		char m_ntl {'C'};
		std::string m_mods {""};  // Todo: Should get rid of this crap
		int m_oct {0};

		friend class spn12tet3;
	};
	using ntl_t = spn12tet3::ntl3_t;

	// Should these be template params for the scale ?
	struct pitch_std {
		pitch_std()=default;
		// These control the pitch standard, ie, the actual frq values of the 
		// named pitches of SPN (C(i),C#(i),D(i),...B(i),C(i+1),...).  
		// Changing ref_ntstr here does _not_ change the note @ which the 
		// octave boundry occurs.  
		ntl3_t ref_ntl {"A"};
		int ref_oct {4};
		frq_t ref_frq {440};
		int gen_int {2};
		int ntet {12};
	};

	// Constructors
	spn12tet3()=default;  // Generates A440 ("A(4)" == 440 Hz)
	spn12tet3(spn12tet3::pitch_std);

	

	std::string name() const;
	std::string description() const;

	// For note-letters derrived from different scales.  The user is responsible
	// for converting their ntl to a string.  This makes it obvious that there
	// are no shenanigans.  That is, it is clear that the comparison only takes 
	// into account the printable ("externally observable") representation of the
	// ntl.  
	bool isinsc(const std::string&) const;
	bool isinsc(frq_t) const;
	std::string print() const;
	std::string print(scd_t,scd_t) const;

	scd_t to_scd(ntl3_t) const;
	std::vector<scd_t> to_scd(const std::vector<ntl3_t>&) const;
	scd_t to_scd(frq_t) const;
	std::vector<scd_t> to_scd(const std::vector<frq_t>&) const;

	frq_t to_frq(ntl3_t) const;
	std::vector<frq_t> to_frq(const std::vector<ntl3_t>&) const;
	frq_t to_frq(scd_t) const;
	std::vector<frq_t> to_frq(const std::vector<scd_t>&) const;

	ntl3_t to_ntl(scd_t) const;
	std::vector<ntl3_t> to_ntl(const std::vector<scd_t>&) const;
	ntl3_t to_ntl(frq_t) const;
	std::vector<ntl3_t> to_ntl(const std::vector<frq_t>&) const;

	// No octave-getters b/c the scd_t and ntl_t's both contain their own
	// octave getters.  
	//
	
private:
	pitch_std m_ps {};
	int m_shift_scd {57};  // the scd that generates the ref frq; 57 => A(4)
	// Info for name(), description()
	std::string m_name {"12-tone chromatic"};
	std::string m_description {"12-tone equal-tempered with A(4)=440Hz; SPN"};
	
};

bool operator!=(const spn12tet3::ntl3_t&, const spn12tet3::ntl3_t&);
bool operator>(const spn12tet3::ntl3_t&, const spn12tet3::ntl3_t&);
bool operator<=(const spn12tet3::ntl3_t&, const spn12tet3::ntl3_t&);
bool operator>=(const spn12tet3::ntl3_t&, const spn12tet3::ntl3_t&);






