	// The int argument specifies which name to return if the interval
	// can be named in more than one way.  In this case, the name 
	// returned is that with idx = the int argument.  
	std::string to_fcname(int=0) const;  // full common name
	std::string to_acname(int=0) const; // abbreviated common name
	std::string whatever(int=0) const; // abbreviated common name
	
	
	// These vectors are the same size. 
	const static std::map<cent_t,std::vector<std::string>> m_cipmap_cent;
	
	
	
	
	
	
	
	
	
	
	
	
//-----------------------------------------------------------------------------
// The cent_t class
const std::map<cent_t,std::vector<std::string>> cent_t::m_cipmap_cent {
	{0_cent,{"u"}}, {100_cent,{"m2"}}, {200_cent,{"M2"}},
	{300_cent,{"m3"}}, {400_cent,{"M3"}},
	{500_cent,{"P4"}}, {600_cent,{"A4","d5"}}, {700_cent,{"P5"}},
	{800_cent,{"m6"}}, {900_cent,{"M6"}},
	{1000_cent,{"m7"}}, {1100_cent,{"M7"}},
	{1200_cent,{"P8","O"}}
};
	
	
	

// Return the abbreviated common name idx indicated by prefer_name
std::string cent_t::whatever(int name_idx) const {
	std::string s {};
	const auto nhundreds = std::div(static_cast<int>(m_cents),100);
	if (nhundreds.rem != 0) { return s; }
	
	if (nhundreds.quot <= 12 && nhundreds.quot >= -12) {
		if (m_cents < 0) { s += "-"; }

		switch (nhundreds.quot) {
			case 0: s += "u"; break;
			case 100: s += "m2"; break;
			case 200: s += "M2"; break;
			case 300: s += "m3"; break;
			case 400: s += "M3"; break;
			case 500: s += "P4"; break;
			case 600: s += name_idx==0 ? "A4" : "d5"; break;
			case 700: s += "P5"; break;
			case 800: s += "m6"; break;
			case 900: s += "M6"; break;
			case 1000: s += "m7"; break;
			case 1100: s += "M7"; break;
			case 1200: s += name_idx==0 ? "P8" : "O"; break;
			default: std::abort(); break;
		}

	} else {
		// If n_hundreds is outside the range [-1200,1200], the interval gets a name like
		// "9'th" (m_cents == 13), "10'th" (m_cents == 14)
		int common_name_number = nhundreds.quot-4;  // magic constant 4
		s += std::to_string(common_name_number) + "'" + int_suffix(common_name_number);
	}

	return s;
}


 // Return the abbreviated common name idx indicated by prefer_name
std::string cent_t::to_acname(int name_idx) const {
	std::string s {};
	const auto nhundreds = std::div(static_cast<int>(m_cents),100);
	if (nhundreds.rem != 0) { return s; }
	
	if (nhundreds.quot <= 12 && nhundreds.quot >= -12) {
		auto map_result = m_cipmap_cent.find(cent_t{std::abs(m_cents)});
		if (map_result == m_cipmap_cent.end()) { return s; }

		auto cent_names = map_result->second;
		if (m_cents < 0) { s += "-"; }
		if (name_idx >= 0 && name_idx < cent_names.size()) {
			s += cent_names[name_idx];
		}
	} else { // abs(m_cents) > 12: 13 cents => 9, 14 cents => 10, ...
		auto common_name_number = static_cast<int>(std::round(nhundreds.quot-4));
		s += std::to_string(common_name_number) + "'" + int_suffix(common_name_number);
	}

	return s;
}


std::string cent_t::to_fcname(int prefer_name) const {
	std::string s {};
	auto acname {to_acname(prefer_name)};  // abbreviated common name
	if (acname.size()==0) { return s; }

	std::map<std::string,std::string> cip_qual_map {{"m","Minor"},{"M","Major"},{"P","Perfect"},
	{"A","Augmented"},{"d","Diminished"},{"u","Unison"},{"O","Octave"}};

	if (m_cents < 0) {
		s += "Descending ";
	} else if (m_cents > 0) {
		s += "Ascending ";
	}

	std::string rx {"(?:-)?([MmAdPuO]{1,1})?(\\d+)?(?:'[thrdndst]{2,2})?"};
	auto rx_caps = rx_match_captures(rx, acname);
	au_assert(rx_caps.has_value(), "!rx_caps");

	if (rx_caps && (*rx_caps)[1]) {
		s += cip_qual_map[*((*rx_caps)[1])] + " ";
	}

	if (rx_caps && (*rx_caps)[2]) { 
		int int_num = std::stoi(*((*rx_caps)[2]));
		s += *((*rx_caps)[2]) + "'" + int_suffix(int_num);
	}

	return s;
}

	
	
std::string cent_t::print(int prefer_name) const {
	std::string s {};
	for (int i=0; true; ++i) {
		auto curr_name = to_acname(i);
		if (curr_name.size()==0) { break; }

		s += curr_name;
		if (prefer_name == i) { s+="*"; }
		s += ", ";
	}
	s.erase(s.end()-2,s.end());
	s += " => " + std::to_string(m_cents) + " cents";

	return s;
}









	
	
	
	