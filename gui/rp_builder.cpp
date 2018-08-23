#include "rp_builder.h"
#include "g_data_pool.h"
#include "aulib\rpgen\rand_rp.h"
#include "aulib\types\types_all.h"
#include "aulib\util\au_util_all.h"
#include <vector>
#include <string>

rp_builder::rp_builder(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	m_pd = m_defaults.pd;  // no setter or ui component yet
	ui.ts->setText(QString().fromStdString(m_defaults.ts));
	set_ts();
	ui.nv_in->setText(QString().fromStdString(m_defaults.curr_nv));
	set_curr_nv();

	ui.n_nts->setText(QString().fromStdString(m_defaults.n_nts));
	set_n_nts();
	ui.n_bars->setText(QString().fromStdString(m_defaults.n_bars));
	set_n_bars();

	for (auto e : m_defaults.common_nvs) {
		if (nv_uih(e).is_valid()) {
			new QListWidgetItem(QString().fromStdString(e), ui.nvpool_addnl);
		}
	}
	
	for (auto e : m_defaults.nv_pool) {
		if (nv_uih(e).is_valid()) {
			new QListWidgetItem(QString().fromStdString(e), ui.nv_pool);
		}
	}
	set_nvpool();

	set_rand_rp_inputs();
}

void rp_builder::on_generate_clicked() {
	set_rand_rp_inputs();
	if (!m_rp_input_status.is_valid) { return; }

	m_rand_rp_result = rand_rp(m_rp_input);
	// rand_rp() returns a std::optional<std::vector<nv_t>>

}

void rp_builder::set_rand_rp_inputs() {
	if (m_ts_uih.is_valid() && m_nnts_uih.is_valid() && m_nbars_uih.is_valid() &&
		m_dp_uih.is_valid() && m_nvpool_uih.is_valid()) {
		m_rp_input = {m_ts_uih.get(),m_nv_pool,m_pd,m_nnts_uih.get(),
			m_nbars_uih.get()};
		m_rp_input_status = rand_rp_check_input(m_rp_input);
	}

	if (!m_rp_input_status.is_valid) {
		// set some kind of message on the window
	}
}

void rp_builder::set_ts() {
	m_ts_uih.update(ui.ts->text().toStdString());
	ui.ts->setToolTip(QString::fromStdString(m_ts_uih.msg()));
	if (!m_ts_uih.is_valid()) {
		ui.ts->setStyleSheet("QLineEdit { background: rgb(255,153,153); }");
	} else {
		ui.ts->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
	}
}
void rp_builder::on_ts_returnPressed() {
	set_ts();
	set_rand_rp_inputs();
}
void rp_builder::on_ts_textEdited() {
	set_ts();
	set_rand_rp_inputs();
}

void rp_builder::set_n_nts() {
	m_nnts_uih.update(ui.n_nts->text().toStdString());
	ui.ts->setToolTip(QString::fromStdString(m_nnts_uih.msg()));
	if (!m_nnts_uih.is_valid()) {
		ui.n_nts->setStyleSheet("QLineEdit { background: rgb(255,153,153); }");
	} else {
		ui.n_nts->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
	}
}
void rp_builder::on_n_nts_returnPressed() {
	set_n_nts();
	set_rand_rp_inputs();
}
void rp_builder::on_n_nts_textEdited() {
	set_n_nts();
	set_rand_rp_inputs();
}

void rp_builder::set_n_bars() {
	m_nbars_uih.update(ui.n_bars->text().toStdString());
	ui.n_bars->setToolTip(QString::fromStdString(m_nbars_uih.msg()));
	if (!m_nbars_uih.is_valid()) {
		ui.n_bars->setStyleSheet("QLineEdit { background: rgb(255,153,153); }");
	} else {
		ui.n_bars->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
	}
}
void rp_builder::on_n_bars_returnPressed() {
	set_n_bars();
	set_rand_rp_inputs();
}
void rp_builder::on_n_bars_textEdited() {
	set_n_bars();
	set_rand_rp_inputs();
}

void rp_builder::set_curr_nv() {
	m_curr_nv.update(ui.nv_in->text().toStdString());
	if (!m_curr_nv.is_valid() && !ui.nv_in->text().isEmpty()) {
		ui.nv_in->setStyleSheet("QLineEdit { background: rgb(255,153,153); }");
	} else {
		ui.nv_in->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
	}
}
void rp_builder::on_nv_in_returnPressed() {
	void set_curr_nv();
	set_rand_rp_inputs();
}
void rp_builder::on_nv_in_textEdited() {
	void set_curr_nv();
	set_rand_rp_inputs();
}
void rp_builder::on_add_nv_clicked() {
	auto nvs_selected = ui.nvpool_addnl->selectedItems();
	for (int i=0; i<nvs_selected.count(); ++i) {
		new QListWidgetItem(nvs_selected[i]->text(), ui.nv_pool);
	}

	if (!(ui.nv_in->text().isEmpty())) {
		new QListWidgetItem(ui.nv_in->text(), ui.nv_pool);
	}

	set_nvpool();
	set_rand_rp_inputs();
}

void rp_builder::on_remove_nv_clicked() {
	auto nvs_selected = ui.nv_pool->selectedItems();
	for (int i=0; i<nvs_selected.count(); ++i) {
		delete nvs_selected[i];
	}
	set_nvpool();
	set_rand_rp_inputs();
}

void rp_builder::set_nvpool() {

	// Has to take all the selected nv's from ui-> common_nvs PLUS that
	// entered in curr_nv and move them to ui->nv_pool

	m_nv_pool.clear();
	for (int i=0; i<ui.nv_pool->count(); ++i) {
		auto curr_nv = nv_uih(ui.nv_pool->item(i)->text().toStdString());
		if (curr_nv.is_valid()) {
			m_nv_pool.push_back(curr_nv);
		}
	}

	ui.nv_pool->clear();
	for (auto e : m_nv_pool) {
		new QListWidgetItem(QString().fromStdString((*(e.get())).print()), ui.nv_pool);
	}
}

void rp_builder::on_import_btn_clicked() {
	if (!m_rand_rp_result) {
		// Error message box saying can't import "nothing"
		return;
	}

	gdp.create(*m_rand_rp_result,std::string{"yay"});
	this->close();
}

void rp_builder::on_cancel_clicked() {
	this->close();
}

