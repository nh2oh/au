#include <string>
#include <vector>
#include <optional>
#include <cmath>
#include <algorithm>
#include "scale_diatonic12tet.h"
#include "scale_12tet.h"
#include "..\types\cent_oct_t.h"
#include "..\types\ntl_t.h"
#include "..\types\frq_t.h"
#include "..\types\scd_t.h"
#include "..\util\au_util_all.h"

scale_diatonic12tet::scale_diatonic12tet() {
	scale_12tet sc12tet {"A"_ntl, octn_t{4}, frq_t{440}};
	build_sc(sc12tet,m_scale_ntl,0);
}

scale_diatonic12tet::scale_diatonic12tet(ntl_t base_ntl, int ip_startidx) {
	scale_12tet sc12tet {"A"_ntl, octn_t{4}, frq_t{440}};
	build_sc(sc12tet,base_ntl,ip_startidx);
}

scale_diatonic12tet::scale_diatonic12tet(scale_12tet scin, ntl_t base_ntl, int ip_startidx) {
	scale_12tet sc12tet {"A"_ntl, octn_t{4}, frq_t{440}};
	build_sc(sc12tet,base_ntl,ip_startidx);
}

void scale_diatonic12tet::build_sc(scale_12tet sc12tet, ntl_t base_ntl, int ip_startidx) {
	auto init_scd = sc12tet.to_scd(ntstr_t {base_ntl,octn_t{0}});
	m_ntls.push_back(ntl_t{sc12tet.to_ntstr(*init_scd)});
	m_frqs.push_back(sc12tet.to_frq(*init_scd));
	std::rotate(m_interval_pattern.begin(), m_interval_pattern.begin()+ip_startidx,m_interval_pattern.end());

	auto curr_scd = *init_scd;
	for (int i=0; i<m_n; ++i) {
		curr_scd = curr_scd + scd_t{m_interval_pattern[i]};
		m_ntls.push_back(ntl_t{sc12tet.to_ntstr(curr_scd)});
		m_frqs.push_back(sc12tet.to_frq(curr_scd));
	}

	m_name = "Diatonic scale " + m_scale_ntl.print() + " ...";
	m_description = "Constructed from scale_12tet:  \n";
	m_description += sc12tet.name() + "\n" + sc12tet.description();
}

int scale_diatonic12tet::n() {
	return m_n;
}

std::string scale_diatonic12tet::name() const {
	return m_name;
}
std::string scale_diatonic12tet::description() const {
	return m_description;
}

// This function assumes none of the ntls in the scale are duplicates, so it
// is not applicable to the completely general case of an arbitrary scale
ntstr_t scale_diatonic12tet::to_ntstr(scd_t scd_in) {  // Reads m_default_valid_ntls directly
	rscdoctn_t rscdoctn_in {scd_in, m_n};
	//return ntstr_t {m_ntls[rscdoctn_in.to_rscd().to_int()], rscdoctn_in.to_octn().to_int()};
	return ntstr_t {m_ntls[rscdoctn_in.to_int()], octn_t{scd_in,m_n}};
}

std::optional<ntstr_t> scale_diatonic12tet::to_ntstr(frq_t frq_in) {  // wrapper
	std::optional<scd_t> scd_in = to_scd(frq_in);
	if (!scd_in) {
		return {};
	}
	return to_ntstr(*scd_in);
}

frq_t scale_diatonic12tet::to_frq(scd_t scd_in) {  // Calls frq_eqt() directly
	rscdoctn_t rscdoct_in {scd_in,m_n};
	int rscd = rscdoct_in.to_int();
	double oct = static_cast<double>(octn_t{scd_in,m_n}.to_int());

	//frq_t frq {m_frqs[rscd].to_double() + std::pow(2,oct)};
	frq_t frq {m_frqs[rscd] + oct_t{oct}};
	return frq;
}
std::optional<frq_t> scale_diatonic12tet::to_frq(ntstr_t ntstr_in) {  // wrapper
	std::optional<scd_t> scd_in = to_scd(ntstr_in);
	if (!scd_in) {
		return {};
	}

	return to_frq(*scd_in);
}
std::optional<scd_t> scale_diatonic12tet::to_scd(frq_t frq_in) {  //  Calls n_eqt() directly
	for (auto i=0; i<m_n; ++i) {
		auto n_approx = std::log2(frq_in/m_frqs[i]);
		if (!aprx_int(n_approx)) {//(isapproxint(n_approx,6)) {
			auto oct = static_cast<int>(n_approx);
			//rscdoctn_t ro {scd_t{i},octn_t{oct},m_n};
			//return scd_t {ro};
			return rscdoctn_t{scd_t{i},m_n}.to_scd(octn_t{oct});
		}
	}
	return {};
}

// This function assumes none of the ntls in the scale are duplicates, so it
// is not applicable to the completely general case of an arbitrary scale
std::optional<scd_t> scale_diatonic12tet::to_scd(ntstr_t ntstr_in) {  // Reads m_ntls directly
	auto it = std::find(m_ntls.begin(),m_ntls.end(),ntl_t{ntstr_in});
	if (it == m_ntls.end()) {
		return {};
	}
	scd_t rscd {static_cast<int>(it-m_ntls.begin())};

	return rscdoctn_t{rscd,m_n}.to_scd(octn_t{ntstr_in});
}

octn_t scale_diatonic12tet::to_octn(scd_t scd_in) {
	return octn_t{scd_in,m_n};
}

std::optional<octn_t> scale_diatonic12tet::to_octn(frq_t frq_in) {
	std::optional<scd_t> scd_in = to_scd(frq_in);
	if (!scd_in) {
		return {};
	}
	return to_octn(*scd_in);
}

std::optional<octn_t> scale_diatonic12tet::to_octn(ntstr_t ntstr_in) {
	std::optional<scd_t> scd_in = to_scd(ntstr_in);
	if (!scd_in) {
		return {};
	}
	return to_octn(*scd_in);
}

bool scale_diatonic12tet::isinsc(frq_t frq_in) {
	for (auto i=0; i<m_n; ++i) {
		auto n_approx = std::log2(frq_in/m_frqs[i]);
		if (aprx_int(n_approx)) {//(isapproxint(n_approx,6)) {
			return true;
		}
	}
	return false;
}

bool scale_diatonic12tet::isinsc(ntl_t ntl_in) {
	return ismember(ntl_in,m_ntls);
}

