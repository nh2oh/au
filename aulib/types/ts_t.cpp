#include "ts_t.h"
#include "beat_bar_t.h"
#include "note_value_t.h"
#include "..\util\au_util_all.h"
#include <string>
#include <optional>

//-----------------------------------------------------------------------------
// The ts_t class

ts_t::ts_t(beat_t bt_per_br_in, note_value nv_per_bt, bool is_compound_in) {
	au_assert(bt_per_br_in.to_double()>0.0); // nv_per_bt is always > 0
	m_compound = is_compound_in;
	if (!is_compound_in) {
		// Simple meter
		m_bpb = bt_per_br_in;
		m_beat_unit = nv_per_bt;
	} else {
		// Compound meter
		// bt_per_br_in is really the "number of 3-part divisions of the beat" per bar
		m_bpb = beat_t {bt_per_br_in.to_double()/3.0};
		// nv_per_bt is really the "note-value per 3-part beat subdivision"
		m_beat_unit = note_value{(nv_per_bt+nv_per_bt).to_double(),1};
	}
}

ts_t::ts_t(std::string str_in) {
	from_string(str_in);
}


// This shouldn't call validate_ts_str() and go through all the "helper"
// bullshit:  It should simply parse the string in the most unforgiving
// way and crash on failure.  No leading/trailing spaces, etc.  
void ts_t::from_string(std::string str_in) {  // Delegated constructor
	auto o_matches = rx_match_captures("(\\d+)/(\\d+)(c)?",str_in);  // std::string{literal_in}
	if (!o_matches || (*o_matches).size() != 4) {
		au_error("Could not parse ts string literal");
	}
	auto matches = *o_matches;

	double bt_per_bar {std::stod(*(matches[1]))};
	double inv_nv_per_bt {std::stod(*(matches[2]))};
	au_assert((bt_per_bar > 0 && inv_nv_per_bt > 0), "No (-) values in a ts");
	
	bool is_compound {false};
	if (matches[3]) { is_compound = true; }

	m_beat_unit = note_value {1.0/inv_nv_per_bt};
	m_bpb = beat_t {bt_per_bar};
	m_compound = is_compound;

	/*ts_str_helper result = validate_ts_str(str_in);
	au_assert(result.is_valid, result.msg);
	m_beat_unit = note_value {1.0/result.inv_nv_per_bt};
	m_bpb = beat_t {result.bt_per_bar};
	m_compound = result.is_compound;*/
}

ts_t operator""_ts(const char *literal_in, size_t length) {
	return ts_t {std::string {literal_in}};
}

note_value ts_t::beat_unit() const {
	return m_beat_unit;
}
note_value ts_t::bar_unit() const {
	return note_value{(m_beat_unit.to_double())*(m_bpb.to_double())};
}
beat_t ts_t::beats_per_bar() const {
	return m_bpb;
}
std::string ts_t::print() const {
	int numerator {0};
	int denominator {0};
	std::string compound_indicator {""};
	if (!m_compound) {
		numerator = static_cast<int>(m_bpb.to_double());
		denominator = static_cast<int>(1/(m_beat_unit.to_double()));
	} else {
		auto o_denominator = m_beat_unit.undot_value();
		if (o_denominator) {
			numerator = 3*static_cast<int>(m_bpb.to_double());
			denominator = static_cast<int>(2.0/(*o_denominator));
			compound_indicator = "c";
		} else {
			return ("?/?");
		}
	}
	std::string s = std::to_string(numerator) + "/" 
		+ std::to_string(denominator) + 
		compound_indicator;
	return s;
}

bool ts_t::operator==(ts_t const& rhs) const {
	return ((m_bpb == rhs.m_bpb) && 
		(m_beat_unit == rhs.m_beat_unit) &&
		(m_compound == rhs.m_compound));
}

bool operator!=(ts_t const& lhs, ts_t const& rhs) {
	return !(lhs == rhs);
}


//-----------------------------------------------------------------------------
// Non-class helper functions
/*
ts_str_helper validate_ts_str(std::string const& str_in) {
	ts_str_helper result { };

	auto o_matches = rx_match_captures("^\\s*([1-9]+)/([1-9]+)(c)?\\s*$",str_in);
	if (!o_matches || (*o_matches).size() != 4) {
		result.is_valid = false;
		result.msg += "A ts must conform to n/d or n/dc where n, d are integers ";
		result.msg += " and \"c\" is the ASCII character c.  ";
		return result;
	}
	auto matches = *o_matches;

	result.is_valid = true;
	result.str_clean = *(matches[1]) + "/" + *(matches[2]);
	result.bt_per_bar = std::stod(*(matches[1]));
	result.inv_nv_per_bt = std::stod(*(matches[2]));
	if (matches[3]) {
		result.is_compound = true;
		result.str_clean += "c";
	}

	return result;
}*/


//-----------------------------------------------------------------------------
// The ts_uih class

ts_uih::ts_uih() {
	//...
}

ts_uih::ts_uih(std::string const& str_in) {
	// Do not set str_last_ before calling update()... update() checkes 
	// str_last_ before doing anything and will not proceed if 
	// str_in == str_last_.  
	update(str_in);
}

void ts_uih::update(std::string const& str_in) {
	if (str_in == str_last_) {
		return;
	}
	str_last_ = str_in;

	auto str_in_parts = parse_ts_str_();
	if (!str_in_parts) {
		is_valid_ = false;
		msg_ += "A ts is of the form n/d or n/dc where n and d are integers ";
		msg_ += " > 0 and \"c\" is the ASCII character c.  ";
		return;
	}

	auto ts_parts = *str_in_parts;
	// Addnl tests... bt_per_bar, inv_nv_per_bt should not have weird magnitudes...

	is_valid_ = true;
	ts_ = ts_t{beat_t{ts_parts.bt_per_bar},note_value{ts_parts.nv_per_bt},
		ts_parts.is_compound};

}

bool ts_uih::is_valid() const {
	return is_valid_;
}

int ts_uih::flags() const {
	return flags_;
}

ts_t ts_uih::get() const {
	au_assert(is_valid_,"Called ts_uih::get() on a ts_uih object with is_valid_==false");
	return *ts_;
}

// Parses str_last_; does not read any other internal state, does not alter
// any internal state.  
// Called by the constructor.  
std::optional<ts_uih::ts_str_parts> ts_uih::parse_ts_str_() const {
	auto o_matches = rx_match_captures("^\\s*([1-9]+)/([1-9]+)(c)?\\s*$",str_last_);
	if (!o_matches || (*o_matches).size() != 4) {
		return {};
	}

	auto matches = *o_matches;
	
	auto bt_per_bar = std::stod(*(matches[1]));
	auto nv_per_bt = 1.0/std::stod(*(matches[2]));
	bool is_compound = false;
	if (matches[3]) {
		bool is_compound = true;
	}
	
	return ts_str_parts {bt_per_bar, nv_per_bt, is_compound};
}


