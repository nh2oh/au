#include "metg_uih.h"
#include "metg_t.h"
#include "ts_t.h"
#include "nv_t.h"
#include "beat_bar_t.h"
#include "rp_t.h"
#include "teejee.h"
#include "..\util\au_error.h"
#include "..\util\au_random.h"
#include "dbklib\algs.h"
#include "..\util\au_util.h"  // wait()
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <numeric> // lcm, gcd, accumulate() for summing prob vectors


metg_uih<metg_input_rp>::metg_uih() {};
metg_uih<metg_input_nvph>::metg_uih() {};
metg_uih<metg_input_pg>::metg_uih() {};


void metg_uih<metg_input_rp>::update(metg_input_rp uinput) {
	if (uinput == m_uinput_last && !m_is_default) { return; }
	m_is_default = false;
	m_uinput_last = uinput;
	m_msg.clear();

	if (m_uinput_last.ts == m_uinput_last.rp.ts() && 
		m_uinput_last.rp.nbeats() > 0_bt) {
		m_is_valid = true;
		m_msg = "";
		m_metg = tmetg_t {m_uinput_last.ts, m_uinput_last.rp};
	} else {
		m_is_valid = false;
		m_msg = "Sorry bro, the ts's have to match and the rp can't be empty";
	}
}

void metg_uih<metg_input_nvph>::update(metg_input_nvph uinput) {
	if (uinput == m_uinput_last && !m_is_default) { return; }
	m_is_default = false;
	m_uinput_last = uinput;
	m_msg.clear();

	if (m_uinput_last.nv.size() > 0 && 
		m_uinput_last.nv.size() == m_uinput_last.ph.size()) {
		m_is_valid = true;
		m_msg = "";
		m_metg = tmetg_t {m_uinput_last.ts, m_uinput_last.nv, m_uinput_last.ph};
	} else {
		m_is_valid = false;
		m_msg = "Sorry bro, the there has to be >= 1 nv and each must have its phase specified";
	}
}

void metg_uih<metg_input_pg>::update(metg_input_pg uinput) {
	if (uinput == m_uinput_last && !m_is_default) { return; }
	m_is_default = false;
	m_uinput_last = uinput;
	m_msg.clear();

	if (m_uinput_last.nvph.size() == 0 || 
		m_uinput_last.pg.size() < 1) {
		m_is_valid = false;
		m_msg = "Need >= 1 nvph and a non-empty pg :(";
		return;
	}

	for (int c=0; c<m_uinput_last.pg.size(); ++c) {
		if (m_uinput_last.pg[c].size() != m_uinput_last.nvph.size()) {
			m_is_valid = false;
			m_msg = "All cols of the pg must have nvph.size() rows";
			return;
		}
	}

	std::vector<d_t> nvs {};
	std::vector<beat_t> phs {};
	for (auto e : m_uinput_last.nvph) {
		nvs.push_back(e.nv);
		phs.push_back(nbeat(m_uinput_last.ts,e.ph));
	}
	tmetg_t dummy_metg {m_uinput_last.ts,nvs,phs};
	auto zps = dummy_metg.find_internal_zero_pointers(m_uinput_last.pg);
	if (zps.size() > 0) {
		m_is_valid = false;
		m_msg = "Zero pointers!\n\t";
		for (auto e : zps) {
			m_msg += "beat" + std::to_string(e[0]) + ", level " + std::to_string(e[1]) + "; ";
		}
		return;
	}

	// Success
	m_metg = tmetg_t {m_uinput_last.ts, m_uinput_last.nvph, m_uinput_last.pg};
	m_is_valid = true;
	m_msg = "";
}

/*
bool metg_uih::is_valid() const { 
	return m_is_valid;
}
std::string metg_uih::msg() const {
	return m_msg; 
}
tmetg_t metg_uih::get() const {
	return m_metg;
}
*/





