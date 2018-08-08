#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_rp_builder.h"
#include "aulib\types\types_all.h"
#include <vector>

class rp_builder : public QMainWindow {
	Q_OBJECT
public:
	rp_builder();
private:
	ts_str_helper m_ts {};
	nvset_str_helper m_nvpool {};
	nv_str_helper m_curr_nv {};
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

