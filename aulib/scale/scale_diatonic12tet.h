#pragma once
#include <vector>
#include <optional>
#include "..\types\frq_t.h"
#include "..\types\ntl_t.h"
#include "..\types\scd_t.h"
#include "scale_12tet.h"


class scale_diatonic12tet {
public:
	// Constructors -- all delegate to build_sc()
	scale_diatonic12tet();
	scale_diatonic12tet(ntl_t,int); // base ntl, ip idx
	explicit scale_diatonic12tet(scale_12tet,ntl_t,int);

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
	void build_sc(scale_12tet,ntl_t,int);  // Delegated constructor

	int m_n = 7;
	std::vector<int> m_interval_pattern {2,2,1,2,2,2,1}; // Default => Major
	ntl_t m_scale_ntl {"C"};
	std::vector<ntl_t> m_ntls {};
	std::vector<frq_t> m_frqs {};

	// Info for name(), description()
	std::string m_name {};
	std::string m_description {};
};
