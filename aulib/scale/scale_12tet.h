#pragma once
#include <vector>
#include <optional>
#include "..\types\cent_oct_t.h"
#include "..\types\frq_t.h"
#include "..\types\ntl_t.h"
#include "..\types\scd_t.h"

//
// TODO:  Scales need overloads for .to_T for vector<U!=T>
// TODO:  Drop the <optional> returns
//

//
// The scale begins on the note 'C', hence octave increments occur between
// B and C.  
//
//

class scale_12tet {
public:
	// Constructors
	scale_12tet();  // Generates A440 ("A" octave 4 == 440 Hz)
	explicit scale_12tet(ntl_t,octn_t,frq_t); // ref_ntl, ref_oct, ref_frq

	// Info
	std::string name() const;
	std::string description() const;

	ntstr_t to_ntstr(scd_t);  // * Reads m_default_valid_ntls directly
	std::optional<ntstr_t> to_ntstr(frq_t); // wraps to_scd(frq_in), to_ntstr(scd_in)

	frq_t to_frq(scd_t);  // * Calls frq_eqt() directly
	std::optional<frq_t> to_frq(ntstr_t);  // wraps to_scd(ntstr_t), to_frq(scd_t)

	std::optional<scd_t> to_scd(frq_t); // * Calls n_eqt() directly
	std::optional<scd_t> to_scd(ntstr_t);    // * Reads m_default_valid_ntls directly

	octn_t to_octn(scd_t);
	std::optional<octn_t> to_octn(frq_t);  // wraps to_scd(frq_in), to_octn(scd_t)
	std::optional<octn_t> to_octn(ntstr_t);    // wraps to_scd(ntstr_t), to_octn(scd_t)

	bool isinsc(frq_t);
	bool isinsc(ntl_t);

	int n();  // So the user can feed rscd2scd(), scd2rscd()
private:
	static const int m_gint;  // Generating interval = 2
	static const int m_n;  // ntet = 12
	static const std::vector<ntl_t> m_default_valid_ntls;
	static const int m_default_ref_ntl_idx;  // == 9 => "A"
	static const frq_t m_default_ref_frq;  // 440 Hz
	static const octn_t m_default_ref_octave;  // == 4 Hz

	int m_ref_ntl_idx {m_default_ref_ntl_idx};
	frq_t m_ref_frq {m_default_ref_frq};
	ntl_t m_ref_ntl {m_default_valid_ntls[m_default_ref_ntl_idx]};
	octn_t m_ref_octn {m_default_ref_octave};

	// Info for name(), description()
	std::string m_name {};
	std::string m_description {};
};


// Return a frq vector corresponding to some equal-tempered scale
std::vector<frq_t> frq_eqt(std::vector<int> const&, frq_t const&, int const&,
	int const&);

// Return a frq corresponding to dn on some equal-tempered scale
frq_t frq_eqt(int const&, frq_t const&, int const&, int const&);

// The inverse:  Returns an scd_t from a frq_t.  Single values, 
// not std::vector
double n_eqt(frq_t const& frq_in, frq_t const& ref_frq,int const& ntet,
	int const& gint);


