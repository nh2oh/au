#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_rp_builder.h"
#include "aulib\rpgen\rand_rp.h"
#include "aulib\uih.h"
#include "aulib\types\ts_t_uih.h"
#include "aulib\numeric_simple_uih.h"
#include "aulib\types\types_all.h"
#include "aulib\types\ts_t_uih.h"
#include <vector>
#include <optional> // rand_rp() returns a std::optional<std::vector<nv_t>>
#include <string>
#include <set>

class rp_builder : public QMainWindow {
	Q_OBJECT
public:
	rp_builder(QWidget *parent = Q_NULLPTR);
private:
	struct defaults {
		std::string ts {"4/4"};
		std::string curr_nv {""};
		std::string n_nts {"0"};
		std::string n_bars {"2"};

		std::set<nv_t> common_nvs {nv_t {1,0}, nv_t {2,0}, nv_t {4,0}};

		std::set<nv_t> nv_pool {nv_t {0.0625,0}, nv_t {0.125,0}, nv_t {0.25,0}, nv_t {0.5,0}};
		std::vector<double> pd {1,1,1,1};
	};
	defaults m_defaults {};

	au::uih_parser<parse_userinput_ts> m_ts_parser {parse_userinput_ts {},
		"The time signature (format: n/d[c] where c means \"compound\")"};
	au::uih<decltype(m_ts_parser)> m_ts_uih {m_ts_parser};

	au::uih_parser<parse_userinput_int> m_nnts_parser {parse_userinput_int {},
		"Constrain the generated sequence to contain this many notes"};
	au::uih_pred<ftr_geq> p_geqzero {ftr_geq{0.0},"Must be >= 0"};
	au::uih<decltype(m_nnts_parser),decltype(p_geqzero)> m_nnts_uih {m_nnts_parser,p_geqzero};

	au::uih_parser<parse_userinput_double> m_nbars_parser {parse_userinput_double {},
		"Number of bars to generate"};
	au::uih<decltype(m_nbars_parser),decltype(p_geqzero)> m_nbars_uih {m_nbars_parser,p_geqzero};

	nv_uih m_curr_nv;
	std::set<nv_t> m_nv_pool;
	std::vector<double> m_pd;  // TODO:  Convert to helper

	randrp_input m_rand_rp_input {};
	randrp_input_check_result m_rand_rp_input_status {};
	std::optional<std::vector<nv_t>> m_rand_rp_result;  // output of rand_rp()

	void set_ts();
	void set_curr_nv();
	void set_nvpool();
	void set_rand_rp_inputs();
	void set_n_nts();
	void set_n_bars();
	//void set_pd();

	Ui::rp_builder ui;
private slots:

	void on_ts_returnPressed();
	void on_ts_textEdited();

	void on_n_nts_returnPressed();
	void on_n_nts_textEdited();
	void on_n_bars_returnPressed();
	void on_n_bars_textEdited();

	void on_nv_in_returnPressed();
	void on_nv_in_textEdited();

	void on_add_nv_clicked();
	void on_remove_nv_clicked();

	void on_generate_clicked();
	void on_import_btn_clicked();
	void on_cancel_clicked();
};

