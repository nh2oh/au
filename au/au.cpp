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
#include <filesystem>
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
	std::chrono::milliseconds res, size_t error) {
	decltype(res/res) one = {1};
	for (auto& e : dt) {  // This loop assigns to dt!  e != const
		auto r = e/res;	
		// "round():" round up if e is exactly betweeen r*res and (r+1)*res;
		// round up if e < res => never round e to 0.  
		(e-r*res <= ((r+1)*res-e)) ? r=r : r=(r+1);  

		// "error = abs(e-e*res)"
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

/*
	size_t rng_a {1103515245}; size_t rng_c {12345};  // Crude random numbers: (a*seed+c)%maxval

	size_t min_nbars = 1; size_t max_nbars = 5;
	for (int i=0; i<50; ++i) {
		size_t curr_nbars = (rng_a*i+rng_c)%(max_nbars+1);
		if (curr_nbars < min_nbars) { curr_nbars = min_nbars; }
		size_t curr_nnts = (rng_a*i+rng_c)%(8*curr_nbars+1);
		if (curr_nnts < 4*curr_nbars) { curr_nnts = 4*curr_nbars; }
		std::cout << "{"<<curr_nnts<<","<<curr_nbars<<"_br},";
	}

	*/

int main(int argc) {
	ts_t ts1 {4_bt,d::q};
	std::vector<scd_t> vs1 {scd_t{2},scd_t{13},scd_t{8},scd_t{10}};
	std::vector<scd_t> vs2 {scd_t{1},scd_t{3},scd_t{5},scd_t{7}};
	std::vector<scd_t> vs3 {scd_t{0},scd_t{2},scd_t{4},scd_t{6},scd_t{8}};
	std::vector<scd_t> vs4 {scd_t{0},scd_t{2},scd_t{4},scd_t{0},scd_t{6},scd_t{8}};
	std::vector<ntl_t> vn1 {"C"_ntl,"E"_ntl,"G"_ntl,"F#"_ntl};
	std::vector<ntl_t> vn2 {"A"_ntl,"B"_ntl,"C#"_ntl,"D#"_ntl};
	chord_t<scd_t> c1 {vs1};
	chord_t<scd_t> c2 {vs2};
	chord_t<scd_t> c3 {vs3};
	chord_t<ntl_t> c4 {vn1};

	musel_t d_c1 {c1,false};
	musel_t d_c2 {c2,false};
	musel_t d_c3 {c3,false};
	musel_t d_c4 {c4,false};
	musel_t d_s1 {scd_t{4},false};
	
	std::cout << d_c1.print() << std::endl;

	std::vector<d_t> vdt1 {d::q,d::h,d::q,d::q,d::q,d::h};
	std::vector<d_t> vdt2 {d::q,d::h,d::q,d::h};
	rp_t rp1 {ts1,vdt1};

	std::vector<decltype(d_c1)> vde1 {d_c1,d_c1,d_c2,d_c2,d_c3,d_c1};
	std::vector<musel_t<scd_t>> vde2 {d_c1,d_s1,d_c2,d_c2,d_c3,d_c1};
	std::vector<musel_t<ntl_t>> vde3 {d_c4,musel_t{vn2[2],false},d_c4,d_c4};

	line_t l1 {ts1,vde1,vdt1};
	line_t l2 {vs4,rp1};
	line_t l3 {ts1,vde2,vdt1};
	line_t l4 {ts1,vde3,vdt2};

	std::cout << l3.print() << std::endl << std::endl; 
	std::cout << l4.print() << std::endl; 
	wait();
	/*
	struct test_set_maker {
		int num_nts {0};
		int num_bars {0};
		bool operator==(test_set_maker& rhs) { return (num_nts==rhs.num_nts && num_bars==rhs.num_bars); };
		bool operator<(test_set_maker& rhs) {
			if (num_bars!=rhs.num_bars) {
				return num_bars<rhs.num_bars;
			} else {
				return num_nts<rhs.num_nts;
			}
		}
	};
	std::vector<test_set_maker> test_sets_vec {};
	int min_nbars = 1; int max_nbars = 5; int min_npb = 4; int max_npb = 16;
	auto nbars = urandi(200,min_nbars,max_nbars);
	std::vector<int> nnts {};
	std::vector<test_set_maker> tests {};
	for (auto e : nbars) {
		tests.push_back({urandi(1,min_npb*e,max_npb*e)[0],e});
	}
	tests = unique(tests);
	for (auto e : tests) {
		std::cout << "{"<<e.num_nts<<","<<e.num_bars<<"_br},";
	}*/

	/*
	ts_t ts1 {3_bt,d::q};
	ts_t ts2 {4_bt,d::q};
	rp_t rp1 {ts2, {d::q,d::q,d::w,d::dw,d::e,d::e,d::h}};
	std::vector<std::vector<double>> pg1 {
			{1,0,0,0,  1,0,0,0},
			{1,0,1,0,  1,0,1,0},
			{1,0,1,0,  1,1,1,1}
	};
	pg1 = transpose(pg1);
	std::vector<std::vector<double>> pg2 {
			{1,0,0,0,  1,0,0,0},
			{1,0,1,0,  1,0,1,0},
			{1,1,1,1,  1,1,1,1}
	};
	pg2 = transpose(pg2);
	std::vector<teejee::nv_ph> vnvph1 {{d::w,d::z},{d::h,d::z},{d::q,d::z}};
	//metg_input_rp uin1 {ts1,rp1};
	//metg_input_rp uin2 {ts2,rp1};
	//metg_uih<metg_input_rp> mguih {};
	//metg_input_nvph uin1 {ts2,{d::h,d::q,d::e,d::sx},{}};
	//metg_input_nvph uin2 {ts2,{d::h,d::q,d::e,d::sx},{-1*1_bt,0_bt,0_bt,0_bt}};
	//metg_uih<metg_input_nvph> mguih {};
	metg_input_pg uin1 {ts2,vnvph1,pg1};
	metg_input_pg uin2 {ts2,vnvph1,pg2};
	metg_uih<metg_input_pg> mguih {};

	std::cout << mguih.msg() << std::endl;
	mguih.update(uin1);
	std::cout << mguih.msg() << std::endl;
	mguih.update(uin2);
	std::cout << mguih.msg() << std::endl;
	mguih.update(uin1);
	std::cout << mguih.msg() << std::endl;
	*/


	/*std::vector<std::vector<double>> pg1 {
		{1,0,0,0,  1,0,0,0},
		{1,0,1,0,  1,0,1,0},
		{1,1,1,1,  1,1,1,1}
	};
	auto pg1t = transpose(pg1);
	tmetg_t mg4 {ts,tg.levels(),pg1t};
	std::cout << mg4.print() << std::endl << std::endl;*/

    return 0;
	
}

