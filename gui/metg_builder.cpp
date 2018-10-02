#include "metg_builder.h"
#include "g_data_pool.h"
#include <vector>


metg_builder::metg_builder(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);
}


void metg_builder::on_normalize_btn_clicked() {
	//...
}

void metg_builder::on_import_btn_clicked() {
	this->close();
}

void metg_builder::on_cancel_btn_clicked() {
	this->close();
}
