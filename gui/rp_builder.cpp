#include "rp_builder.h"
#include "g_data_pool.h"
#include "aulib\rpgen\rand_rp.h"
#include "aulib\types\types_all.h"
#include "aulib\util\au_util_all.h"

rp_builder::rp_builder(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	m_pd = {1,1,1,1,1};  // no setter or ui component yet
	ui.ts->setText(QString().fromStdString(defaults_.ts));
	set_ts();
	ui.nv_in->setText(QString().fromStdString(defaults_.curr_nv));
	set_curr_nv();

	ui.n_nts->setText(QString().fromStdString(defaults_.n_nts));
	ui.n_bars->setText(QString().fromStdString(defaults_.n_bars));

	for (auto e : defaults_.common_nvs) {
		if (nv_uih(e).is_valid()) {
			new QListWidgetItem(QString().fromStdString(e), ui.nvpool_addnl);
		}
	}
	
	for (auto e : defaults_.nv_pool) {
		if (nv_uih(e).is_valid()) {
			new QListWidgetItem(QString().fromStdString(e), ui.nv_pool);
		}
	}
	set_nvpool();

	set_rand_rp_inputs();
}

void rp_builder::on_generate_click() {
	set_rand_rp_inputs();

	if (m_rand_rp_inputs.is_valid) {
		auto rp_result = rand_rp(ts_.get(),m_nvpool.nvset,m_pd,m_rand_rp_inputs.nnts,
			m_rand_rp_inputs.n_bars);
		if (rp_result) {
			m_rp_result = *rp_result;
		}
	}
}

void rp_builder::set_rand_rp_inputs() {
	if (ts_.is_valid() && m_nvpool.is_valid) {
		int nnts = std::stoi(ui.n_nts->text().toStdString());
		bar_t n_bars {std::stod(ui.n_bars->text().toStdString())};
		m_rand_rp_inputs = validate_rand_rp_input(ts_.get(),m_nvpool.nvset,m_pd,
			nnts,n_bars);
	}
}

void rp_builder::set_ts() {
	ts_.update(ui.ts->text().toStdString());
	if (!ts_.is_valid()) {
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
	curr_nv_.update(ui.nv_in->text().toStdString());
	if (!curr_nv_.is_valid() && !ui.nv_in->text().isEmpty()) {
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
		auto curr_nv = nv_uih(ui.nv_pool->item(i)->text().toStdString());
		if (curr_nv.is_valid() && !ismember(curr_nv.get(),nv_pool)) {
			nv_pool.push_back(curr_nv.get());
		}
	}
	// The ismember() condition is actually a condition imposed by rand_rp()
	// and should probably be enforced through the rand_rp_uih.  

	ui.nv_pool->clear();

	for (auto e : nv_pool) {
		new QListWidgetItem(QString().fromStdString(e.print()), ui.nv_pool);
	}
}

void rp_builder::on_add_nv_click() {
	auto nvs_selected = ui.nvpool_addnl->selectedItems();
	for (int i=0; i<nvs_selected.count(); ++i) {
		new QListWidgetItem(nvs_selected[i]->text(), ui.nv_pool);
	}

	if (!(ui.nv_in->text().isEmpty())) {
		new QListWidgetItem(ui.nv_in->text(), ui.nv_pool);
	}

	set_nvpool();
}

void rp_builder::on_remove_nv_click() {
	auto nvs_selected = ui.nv_pool->selectedItems();
	for (int i=0; i<nvs_selected.count(); ++i) {
		delete nvs_selected[i];
	}
	set_nvpool();
}

void rp_builder::on_import_2_click() {
	//...
}

void rp_builder::on_cancel_click() {
	//...
}

