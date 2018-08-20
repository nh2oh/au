#pragma once
#include <QtWidgets/QMainWindow>
#include "aulib\rpgen\rand_rp.h"
#include "ui_rp_builder.h"
#include "aulib\types\types_all.h"
#include "aulib\types\ts_t_uih.h"
#include <vector>
#include <string>

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

		std::vector<std::string> common_nvs {"2/1", "1/1", "1/2", "1/2.", "1/4",
			"1/4.", "1/8", "1/8.","1/16", "1/16.","1/32"};

		std::vector<std::string> nv_pool {"1/4", "1/8", "1/16"};
		std::vector<double> pd {1,1,1};
	};
	defaults defaults_ {};
	/*
	uih_parser<parse_userinput_ts> m_ts_parser {parse_userinput_ts {},
		"The format of a ts is n/d[c] where n,d are both integers > 0."};
	uih<uih_parser<parse_userinput_ts>> m_ts_uih {m_ts_parser};*/
	//ts_uih ts_;
	nv_uih curr_nv_;
	std::vector<nv_uih> nv_pool_;
	//randrp_uih randrp_input_;
	std::vector<double> pd_;  // Convert to helper
	int n_nts_;  // Convert to helper
	bar_t n_bars_;  // Convert to helper
	std::vector<nv_t> rp_result_;


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
	void on_nv_in_returnPressed();
	void on_nv_in_textEdited();

	void on_add_nv_clicked();
	void on_remove_nv_clicked();

	void on_generate_clicked();
	void on_import_2_clicked();
	void on_cancel_clicked();
};

