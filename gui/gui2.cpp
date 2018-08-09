#include "gui2.h"
#include "aulib\util\au_util_all.h"
#include "nf_import_window.h"
#include "rp_builder.h"
#include "g_data_pool.h"
#include <string>

gui2::gui2(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);
	
}

void gui2::on_actionImport_triggered() {
	nf_import_window *nfimport_w = new nf_import_window(this);
	nfimport_w->setAttribute(Qt::WA_DeleteOnClose);
	nfimport_w->show();
}

void gui2::on_actionRandrp_triggered() {
	rp_builder *rp_builder_w = new rp_builder(this);
	rp_builder_w->setAttribute(Qt::WA_DeleteOnClose);
	rp_builder_w->show();
}

void gui2::on_update_clicked() {
	auto all_nfs = gdp.list_all();
	ui.gdata_list->clear();
	for (auto e : all_nfs) {
		new QListWidgetItem(QString().fromStdString(e), ui.gdata_list);
	}

	wait();
}

