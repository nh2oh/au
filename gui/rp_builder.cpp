#include "rp_builder.h"
#include "g_data_pool.h"
#include "aulib\rpgen\rand_rp.h"
#include "aulib\types\types_all.h"

rp_builder::rp_builder() {
	ui.setupUi(this);

	m_pd = {1,1,1,1,1};  // no setter or ui component yet
	ui.ts->setText("4/4");
	set_ts();

	ui.n_nts->setText("0");
	ui.n_bars->setText("2");

	std::vector<note_value> def_nvpool_addnl {"1/2."_nv, "1/4."_nv, "1/8."_nv, "1/16."_nv, "1/32"_nv};
	for (auto e : def_nvpool_addnl) {
		new QListWidgetItem(QString().fromStdString(e.print()), ui.nvpool_addnl);
	}
	std::vector<note_value> def_nvpool {"1/1"_nv, "1/2"_nv, "1/4"_nv, "1/8"_nv, "1/16"_nv};
	for (auto e : def_nvpool) {
		new QListWidgetItem(QString().fromStdString(e.print()), ui.nv_pool);
	}
	set_nvpool();
	set_rand_rp_inputs();
}

void rp_builder::on_generate_click() {
	set_rand_rp_inputs();

	if (m_rand_rp_inputs.is_valid) {
		auto rp_result = rand_rp(m_ts.ts,m_nvpool.nvset,m_pd,m_rand_rp_inputs.nnts,
			m_rand_rp_inputs.n_bars);
		if (rp_result) {
			m_rp_result = *rp_result;
		}
	}
}

void rp_builder::set_rand_rp_inputs() {
	if (m_ts.is_valid && m_nvpool.is_valid) {
		int nnts = std::stoi(ui.n_nts->text().toStdString());
		bar_t n_bars {std::stod(ui.n_bars->text().toStdString())};
		m_rand_rp_inputs = validate_rand_rp_input(m_ts.ts,m_nvpool.nvset,m_pd,
			nnts,n_bars);
	}
}

void rp_builder::set_ts() {
	m_ts = validate_ts_str(ui.ts->text().toStdString());
	if (!m_ts.is_valid) {
		ui.ts->setStyleSheet("QLineEdit { background: rgb(255,153,153); }");
	} else {
		ui.ts->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
	}
}
void rp_builder::on_ts_returnPressed() {
	void set_ts();
}
void rp_builder::on_ts_textEdited() {
	void set_ts();
}

void rp_builder::set_curr_nv() {
	m_curr_nv = validate_nv_str(ui.nv_in->text().toStdString());
	if (!m_curr_nv.is_valid) {
		ui.nv_in->setStyleSheet("QLineEdit { background: rgb(255,153,153); }");
	} else {
		ui.nv_in->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
	}
}
void rp_builder::on_nv_in_returnPressed() {
	void set_curr_nv();
}
void rp_builder::on_nv_in_textEdited() {
	void set_curr_nv();
}

void rp_builder::set_nvpool() {
	std::vector<note_value> nv_pool {};
	for (int i=0; i<ui.nv_pool->count(); ++i) {
		//auto curr_item = ui.nv_pool->item(i)->text().toStdString();
		auto curr_nv = validate_nv_str(ui.nv_pool->item(i)->text().toStdString());
		if (curr_nv.is_valid) {
			nv_pool.push_back(curr_nv.nv);
		} else {
			// if not valid, set a red background
		}
	}

	m_nvpool = validate_nvset(nv_pool);
	if (!m_nvpool.is_valid) {
		ui.nv_pool->setStyleSheet("QLineEdit { background: rgb(255,153,153); }");
	} else {
		ui.nv_pool->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
	}
}
void rp_builder::on_nv_in_returnPressed() {
	void set_curr_nv();
}
void rp_builder::on_nv_in_textEdited() {
	void set_curr_nv();
}

void rp_builder::on_add_nv_click() {
	std::vector<note_value> new_nvs {};
	auto nvs_selected = ui.nvpool_addnl->selectedItems();
	for (int i=0; i<nvs_selected.count(); ++i) {
		auto curr_nv = validate_nv_str(nvs_selected[i]->text().toStdString());
		if (curr_nv.is_valid) {
			new_nvs.push_back(curr_nv.nv);
		}
	}

	// Append new_nvs to pre-existing nvs
	//validate_nvset(total_nvset);
	// Add all to nv_pool, even if there are errors.  It's possible that errors are
	// caused by other form fields:  we want the user to be able to fix things in any
	// order.  
}

void rp_builder::on_remove_nv_click() {
	//...
}

void rp_builder::on_import_2_click() {
	//...
}

void rp_builder::on_cancel_click() {
	//...
}

