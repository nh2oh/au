#pragma once
#include <QtWidgets/QMainWindow>
#include "aulib\rpgen\rand_rp.h"
#include "ui_rp_builder.h"
#include "aulib\types\types_all.h"
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

		/*std::vector<std::string> common_nvs {nv_uih("2/1"), nv_uih("1/1"), nv_uih("1/2"),
			nv_uih("1/2."), nv_uih("1/4"), nv_uih("1/4."), nv_uih("1/8"),
			nv_uih("1/8."), nv_uih("1/16"), nv_uih("1/16."), nv_uih("1/32")};*/
		std::vector<std::string> common_nvs {"2/1", "1/1", "1/2", "1/2.", "1/4",
			"1/4.", "1/8", "1/8.","1/16", "1/16.","1/32"};

		std::vector<std::string> nv_pool {"1/4", "1/8", "1/16"};
	};
	defaults defaults_ {};

	ts_uih ts_;
	nv_uih curr_nv_;
	//std::vector<nv_uih> nvpool_;
	//std::vector<nv_uih> common_nvs_;
	//-------------------------------------------
	//nvset_str_helper m_nvpool {};
	//nv_str_helper m_curr_nv {};
	rand_rp_input_helper m_rand_rp_inputs {};
	std::vector<note_value> m_rp_result;
	std::vector<double> m_pd;

	void set_ts();
	void set_curr_nv();
	void set_nvpool();
	void set_rand_rp_inputs();

	Ui::rp_builder ui;
private slots:

	void on_ts_returnPressed();
	void on_ts_textEdited();
	void on_nv_in_returnPressed();
	void on_nv_in_textEdited();

	void on_add_nv_click();
	void on_remove_nv_click();

	void on_generate_click();
	void on_import_2_click();
	void on_cancel_click();
};

