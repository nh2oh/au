#include "rp_builder.h"
#include "g_data_pool.h"
#include "aulib\rpgen\rand_rp.h"
#include "aulib\types\types_all.h"
#include "aulib\util\au_util_all.h"
#include <vector>
#include <string>

rp_builder::rp_builder(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	pd_ = defaults_.pd;  // no setter or ui component yet
	ui.ts->setText(QString().fromStdString(defaults_.ts));
	set_ts();
	ui.nv_in->setText(QString().fromStdString(defaults_.curr_nv));
	set_curr_nv();

	ui.n_nts->setText(QString().fromStdString(defaults_.n_nts));
	set_n_nts();
	ui.n_bars->setText(QString().fromStdString(defaults_.n_bars));
	set_n_bars();

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

void rp_builder::on_generate_clicked() {
	set_rand_rp_inputs();

	if (randrp_input_.is_valid()) {
		auto randrp_input = randrp_input_.get();
		auto rp_result = rand_rp(randrp_input.ts,randrp_input.nvset,randrp_input.pd,randrp_input.n_nts,
			randrp_input.n_bars);
		if (rp_result) {
			rp_result_ = *rp_result;
			ui.rp_result->setPlainText(QString().fromStdString(printrp(randrp_input.ts,rp_result_)));
		}
	}
	wait();
}

void rp_builder::set_rand_rp_inputs() {
	randrp_input_.update(ts_, nv_pool_, pd_, n_nts_, n_bars_);
}

void rp_builder::set_n_nts() {
	n_nts_ = std::stoi(ui.n_nts->text().toStdString());
}

void rp_builder::set_n_bars() {
	n_bars_ = bar_t {std::stod(ui.n_bars->text().toStdString())};
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
	set_ts();
	set_rand_rp_inputs();
}
void rp_builder::on_ts_textEdited() {
	set_ts();
	set_rand_rp_inputs();
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
	nv_pool_.clear();
	for (int i=0; i<ui.nv_pool->count(); ++i) {
		auto curr_nv = nv_uih(ui.nv_pool->item(i)->text().toStdString());
		if (curr_nv.is_valid()) {
			nv_pool_.push_back(curr_nv);
		}
	}

	ui.nv_pool->clear();
	for (auto e : nv_pool_) {
		new QListWidgetItem(QString().fromStdString(e.get().print()), ui.nv_pool);
	}
}



void rp_builder::on_import_2_clicked() {
	gdp.create(rp_result_,std::string{"yay"});
	this->close();
}

void rp_builder::on_cancel_clicked() {
	this->close();
}

