
/*
//-----------------------------------------------------------------------------
// Constructors


scale::scale(std::string name_in, std::string description_in, 
	std::vector<std::vector<ntl_t>> ntls_in, std::vector<frq_t> frqs_in) {
	m_name = name_in;
	m_description = description_in;
	m_ntls = ntls_in;
	m_frqs = frqs_in;
	m_n = static_cast<int>(frqs_in.size());

	if (!validate()) { au_error("Scale does not validate!"); }
}

//----------------------------------------------------------------------------
// Public functions for returning data from the scale

// Return _all_ ntstrs
std::vector<std::vector<ntl_t>> scale::get_ntls() const {
	return m_ntls;
}
// Return _all_ the frqs
std::vector<frq_t> scale::get_frqs() const {
	return m_frqs;
}

ntstr_t scale::get_ntstr(frq_t const& frq_in) const {
	rscdoct_t ro = get_rscdoct(frq_in);
	return ntstr_t {m_ntls[ro.rscd.to_int()][0],ro.oct};
}
ntstr_t scale::get_ntstr(scd_t const& scd_in) const {
	rscdoct_t ro = scd2rscd(m_n,scd_in);
	return ntstr_t {m_ntls[ro.rscd.to_int()][0],ro.oct};
}

ntl_t scale::get_ntl(frq_t const& frq_in) const {
	rscdoct_t ro = get_rscdoct(frq_in);
	return m_ntls[ro.rscd.to_int()][0];
}
ntl_t scale::get_ntl(scd_t const& scd_in) const {
	rscdoct_t ro = scd2rscd(m_n,scd_in);
	return m_ntls[ro.rscd.to_int()][0];
}

frq_t scale::get_frq(scd_t const& scd_in) const {
	rscdoct_t ro = scd2rscd(m_n,scd_in);
	//return (m_frqs[ro.rscd.to_int()] + ro.oct);
	std::cout << "FIX THIS" << std::endl;
	return (plus_oct(m_frqs[ro.rscd.to_int()],ro.oct));
}
frq_t scale::get_frq(ntstr_t const& ntstr_in) const {
	auto o = ntstr_in.to_oct();
	auto i = get_rscd(ntstr_in.to_ntl()).to_int();
	rscdoct_t ro = rscdoct_t(i,o.to_double());
	//return (m_frqs[ro.rscd.to_int()] + ro.oct);
	std::cout << "FIX THIS" << std::endl;
	return (plus_oct(m_frqs[ro.rscd.to_int()],ro.oct));
}

scd_t scale::get_scd(ntstr_t const& ntstr_in) const {
	scd_t rscd = scale::get_rscd(ntstr_in.to_ntl());
	return rscd2scd(m_n, rscd, ntstr_in.to_oct());
}
scd_t scale::get_scd(frq_t const& frq_in) const {
	rscdoct_t ro = get_rscdoct(frq_in);
	return rscd2scd(m_n,ro.rscd,ro.oct);
}

// Note there's no get_scd(ntl_t) overload since converting an ntl_t to an scd
// involves making an assumption about the octave
scd_t scale::get_rscd(ntl_t const& ntl_in) const {
	bool f_found_ntl{ false };
	for (int i = 0; i < m_n; ++i) {
		if (ismember(ntl_in, m_ntls[i])) {
			f_found_ntl = true;
			return scd_t {i};
		}
	}
	au_error("scale::get_scd(ntl_t const& ntl_in):  Didn't find ntl_in!");
}

rscdoct_t scale::get_rscdoct(frq_t const& frq_in) const {
	rscdoct_t ro {};
	for (int i=0; i<m_n; ++i) {
		double log2_frqr = std::log2(frq_in/m_frqs[i]);
		if (isapproxint(log2_frqr, 6)) {
			// log2_frqr[i] is the octave number, i is the rscd
			ro.oct = oct_t{static_cast<double>(std::round(log2_frqr))}; ro.rscd = i;
			return ro;
		}
	}
	au_error("scale::get_scd(frq_t const& frq_in):  Didn't find frq_in!");
}

std::string scale::get_name() const {
	return m_name;
}

std::string scale::get_description() const {
	return m_description;
}

int scale::n() const {
	return m_n;
}

bool scale::isinsc(ntstr_t const& ntstr_in) const {
	return scale::isinsc(ntstr_in.to_ntl());
}

bool scale::isinsc(ntl_t const& ntl_in) const {
	for (auto c_ntls : m_ntls) {
		if (ismember(ntl_in, c_ntls)) {
			return true;
		}
	}
	return false;
}

bool scale::isinsc(frq_t const& frq_in) const {
	for (auto c_frq : m_frqs) {
		double log2_frqr = std::log2(frq_in/c_frq);
		if (isapproxint(log2_frqr, 6)) {
			return true;
		}
	}
	return false;
}

std::string scale::print(std::string mode) const {
	auto fmt = std::string();
	fmt.append("m_name == %s\nm_description == %s\nn == %d\nNote pitch classes:\n");

	auto bs = boost::format(fmt) % m_name % m_description % m_n;
	auto s = bs.str();

	std::vector<std::string> s_ntstrs{}; s_ntstrs.reserve(m_n);
	std::vector<size_t> n_ntstrs{}; n_ntstrs.reserve(m_n);
	std::vector<std::string> s_scds{}; s_scds.reserve(m_n);
	std::vector<size_t> n_scds{}; n_scds.reserve(m_n);
	std::vector<std::string> s_frqs{}; s_frqs.reserve(m_n);
	std::vector<size_t> n_frqs{}; n_frqs.reserve(m_n);
	boost::format bs_scd{ "%d" };
	boost::format bs_frq{ "%.3f" };

	scd_t cscd {0};
	while (cscd < m_n) {
		auto cntls = m_ntls[cscd.to_int()];
		auto cfrq = m_frqs[cscd.to_int()];
		s_scds.push_back((bs_scd%cscd.to_int()).str());
		n_scds.push_back((s_scds.back()).length());
		s_frqs.push_back((bs_frq%cfrq).str());
		n_frqs.push_back((s_frqs.back()).length());
		s_ntstrs.push_back("{" + print_ntls(cntls) + "}");
		n_ntstrs.push_back((s_ntstrs.back()).length());
		++cscd;
	}
	auto n_scds_max = *std::max_element(n_scds.begin(), n_scds.end());
	auto n_frqs_max = *std::max_element(n_frqs.begin(), n_frqs.end());
	auto n_ntstrs_max = *std::max_element(n_ntstrs.begin(), n_ntstrs.end());

	auto pad_c1 = std::to_string(n_scds_max + 4);
	auto pad_c2 = std::to_string(n_ntstrs_max + 2);
	auto pad_c3 = std::to_string(n_frqs_max + 2);
	fmt = "\t%-" + pad_c1 + "s%-" + pad_c2 + "s%" + pad_c3 + "s Hz\n";

	for (size_t i = 0; i<s_scds.size(); ++i) {
		bs = boost::format(fmt) % s_scds[i] % s_ntstrs[i] % s_frqs[i];
		s.append(bs.str());
	}

	return s;
}


//-----------------------------------------------------------------------------
// Public setters, editing functions

bool scale::set_name(const std::string& name_in) {
	auto init_name = m_name;
	m_name = name_in;
	if (!validate()) {
		m_name = init_name;
		return false;
	}
	return true;
}

bool scale::set_description(const std::string& description_in) {
	auto init_description = m_description;
	m_description = description_in;
	if (!validate()) {
		m_description = init_description;
		return false;
	}
	return true;
}

// setting ntls only on row scd
bool scale::set_ntls(const scd_t& scd, const std::vector<ntl_t>& ntls_in) {
	if ((scd < 0) || (scd > (m_n-1))) {
		return false;
	}
	auto init_ntls = m_ntls[scd.to_int()];
	m_ntls[scd.to_int()] = ntls_in;
	if (!validate()) {
		m_ntls[scd.to_int()] = init_ntls;
		return false;
	}
	return true;
}

// setting frq only on row scd
bool scale::set_frq(const scd_t& scd, const frq_t& frq_in) {
	if ((scd < 0) || (scd > (m_n-1))) {
		return false;
	}

	auto init_frq = m_frqs[scd.to_int()];
	m_frqs[scd.to_int()] = frq_in;
	if (!validate()) {
		m_frqs[scd.to_int()] = init_frq;
		return false;
	}
	return true;
}

// setting ntls and frq on row scd
bool scale::set_npc(const scd_t& scd, const frq_t& frq_in, 
	const std::vector<ntl_t>& ntls_in) {
	if (!set_frq(scd,frq_in)) { return false; }
	if (!set_ntls(scd, ntls_in)) { return false; }

	return true;
}

// insert _before_ scd_t argument
bool scale::insert_npc(const scd_t& scd, const frq_t& frq_in, 
	const std::vector<ntl_t>& ntls_in) {
	if ((scd < 0) || (scd > m_n)) {
		return false;
	}
	auto frqs_init = m_frqs;
	auto ntls_init = m_ntls;
	auto n_init = m_n;
	m_frqs.insert(m_frqs.begin() + scd.to_int(), frq_in);
	m_ntls.insert(m_ntls.begin() + scd.to_int(), ntls_in);
	m_n = static_cast<int>(m_frqs.size());
	if (!validate()) {
		m_frqs = frqs_init;
		m_ntls = ntls_init;
		m_n = n_init;
		return false;
	}
	return true;
}

// Remove a row from the npc table
bool scale::delete_npc(const scd_t& scd) {
	if ((scd < 0) || (scd > m_n)) {
		return false;
	}

	auto frqs_init = m_frqs;
	auto ntls_init = m_ntls;
	auto n_init = m_n;
	m_frqs.erase(m_frqs.begin() + scd.to_int());
	m_ntls.erase(m_ntls.begin() + scd.to_int());
	m_n = static_cast<int>(m_frqs.size());
	if (!validate()) {
		m_frqs = frqs_init;
		m_ntls = ntls_init;
		m_n = n_init;
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
// Private utility functions



// validate():  The operational definition of what it means to be a scale
bool scale::validate() {
	// m_name and m_description must not be empty
	if (m_name.empty() || m_description.empty()) {
		return false;
	}

	// A scale must contain at least 2 npc's; the number of entries in the 
	// frqs and ntls tables must be the same
	if (m_n <= 1 || m_n != m_frqs.size() || m_n != m_ntls.size()) {
		return false;
	}

	// All ntls are unique
	std::vector<ntl_t> all_ntls {}; all_ntls.reserve(2*m_n);
	for (auto const& cnpc : m_ntls) {
		for (auto const& cntl : cnpc) {
			all_ntls.push_back(cntl);
		}
	}
	if (n_unique(all_ntls) != all_ntls.size()) {
		return false;
	}
	
	return true;
}

//-----------------------------------------------------------------------------
// Non-class functions related to working with scales






*/

