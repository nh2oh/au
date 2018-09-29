#include <vector>
#include <string>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <string>
#include <cstdio>
#include <any>
#include <tuple>
#include <type_traits>
#include <functional>
#include <optional>
#include <chrono>
#include "aulib\aulib_all.h"



/*
#include "aulib\uih2.h"

template<typename POType> struct gprsr_res {
	gprsr_res() {};  // Default constructible
	gprsr_res(std::optional<POType> a2, std::string a3, std::string a4) : 
		res(a2), info(a3), msg(a4) {};

	using this_POType = typename POType;

	std::optional<POType> res {};
	const std::string info {"This prsr gives no guidance."};  // NB:  Fixed at construction
	std::string msg {};  // Set by the parser depending on the input
};


template<typename PredIType> struct gpred_res {
	gpred_res() {};
	gpred_res(bool a2, std::string a3, std::string a4) : 
		is_valid(a2), info(a3), msg(a4) {};

	using this_PredIType = typename PredIType;
	bool is_valid {false};
	const std::string info {"This pred gives no guidance."};
	std::string msg {};
};

// gprsr's have no internal state:  all members are const.  
// operator(std::string) returns the gprsr_res, member is_valid(std::string),
// returns a bool.  
template<typename PF> struct gprsr {
	using PF_RT = typename std::invoke_result_t<PF,std::string>;
	using POType = typename PF_RT::this_POType;
	static_assert(std::is_same<PF_RT,gprsr_res<POType>>::value);
	
	gprsr() {};
	PF m_pf;

	gprsr_res<POType> operator()(std::string x) {
		return (m_pf(x));
	};
	bool is_valid(std::string x) {
		if (!(m_pf(x).res)) {return false;}
		return true;
	};
	std::string info() {
		return m_pf("").info;
	};
	std::string msg(std::string x) {
		return m_pf(x).msg;
	};
};



// gpreds's have no internal state:  all members are const.  
// operator(std::string) returns the gpred_res, member is_valid(std::string),
// returns a bool.  
template<typename PF> struct gpred {
	using PIType = typename PF::this_PIType;
	using PF_RT = typename std::invoke_result_t<PF,PIType>;
	static_assert(std::is_same<PF_RT,gpred_res<PIType>>::value);
	
	gpred() {};
	PF m_pf;

	gpred_res<PIType> operator()(PIType x) {
		return (m_pf(x));
	};
	bool is_valid(PIType x) {
		return m_pf(x).is_valid;
	};
	std::string info() {
		return m_pf(PIType{}).info;
	};
	std::string msg(PIType x) {
		return m_pf(x).msg;
	};
};


template<typename PrsF, typename PrdFs> struct uih2 {
	using PrsOType = typename PrsF::POType;
	static_assert(std::is_same<std::invoke_result_t<PrsF,std::string>,gprsr_res<PrsOType>>::value);
	using PrdIType = PrsOType;
	static_assert(std::is_same<std::invoke_result_t<PrdFs,PrdIType>,gpred_res<PrdIType>>::value);

	uih2() { 
		m_info = m_prsf.info() + "\n" + m_prdfs.info();
	};

	PrsF m_prsf;
	PrdFs m_prdfs;

	bool m_is_valid {false};
	std::optional<PrsOType> m_parser_result {};
	std::string m_usrinput_last {};
	PrdIType m_predinput_last {};
	bool m_pred_result {false};
	std::string m_msg {};
	std::string m_info {};

	void parse(std::string usrinput) {
		if (usrinput == m_usrinput_last) { return; }

		m_usrinput_last = usrinput;
		auto pr = m_prsf(usrinput);
		m_msg = pr.msg;
		m_parser_result = pr.res;
		return;
	};

	void eval(PrdIType usrinput) {
		if (usrinput == m_predinput_last) { return; }

		m_predinput_last = usrinput;
		auto pr = m_prdfs(usrinput);
		m_msg = pr.msg;
		m_pred_result = pr.is_valid;
	};

	std::string info() {
		return m_info;
	};

	std::string msg() {
		return m_msg;
	};
};


template<typename T>
struct no_parser_impl {
	gprsr_res<T> operator()(std::string const s) {
		return gprsr_res<T> {T{},"",""};
	};
};
template<typename T>
using no_parser = gprsr<no_parser_impl<T>>;

struct atsprsr {
	gprsr_res<ts_t> operator()(std::string const pinput) {
		gprsr_res<ts_t> pres {{},"a/b[c]",""};
		if (pinput == "4/4") {
			pres.res = ts_t {pinput};
		} else {
			pres.msg = "Nope not a ts";
		}
		return pres;
	};
};


struct atspred {
	gpred_res<ts_t> operator()(ts_t pinput) {
		gpred_res<ts_t> pres {false,"Gotta be 3/4",""};
		if (pinput.print() == "3/4") {
			pres.is_valid = true;
			pres.msg = "";
			return pres;
		}
		pres.msg = std::string{"not 3/4     :("};
		return pres;
	};
};

using ts_parser = gprsr<atsprsr>;
using ts_pred = gpred<atspred>;

//template<typename T>
//using ts_uih = uih2<gprsr<atsprsr>,gpred<T>>;
*/



size_t quantize_and_count_unique(
	std::vector<std::chrono::milliseconds> dt,
	std::chrono::milliseconds const& res, size_t& error) {
	decltype(res/res) one = {1};
	for (auto& e : dt) {
		auto r = e/res;	
		(e-r*res <= ((r+1)*res-e)) ? r=r : r=(r+1);
		if (r == 0) { r = one; }
		if (e > r*res) {
			error += (e-r*res).count();
		} else {
			error += (r*res-e).count();
		}
		e=(r*res);
	}

    std::sort(dt.begin(),dt.end());
    auto last = std::unique(dt.begin(),dt.end());
    return last-dt.begin();
}

int main(int argc) {
	ts_t ts1 {4_bt,d::q};
	teejee tg1 {ts1,{d::w,d::h,d::q},{0_bt,0_bt,0_bt}};

	std::vector<std::vector<double>> pg1 {
		{1,0,0,0},
		{1,0,1,0},
		{1,1,1,1}
	};
	auto pg1t = transpose(pg1);
	auto pg2t = pg1t;

	tmetg_t mg1 {ts1,tg1.levels(),pg1t};
	auto tf = mg1.validate();

	auto mg2 = mg1.slice(0_bt,6_bt);
	mg2.set_length_exact(6_bt);
	auto mg3=mg1; mg3.set_length_exact(6_bt);

	std::cout << mg1.print() << std::endl << std::endl;
	std::cout << mg2.print() << std::endl << std::endl;
	std::cout << mg3.print() << std::endl << std::endl;
    return 0;
	
}

