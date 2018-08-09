#include "gui.h"
#include "nf_import_window.h"
#include "aulib\rpgen\rand_rp.h"
#include "aulib\types\types_all.h"
#include "aulib\util\au_util_all.h"
#include "g_data_pool.h"
#include <string>
#include <optional>
#include <vector>
#include <QtWidgets/QFileDialog>

gui::gui(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);
}

void gui::on_btn_make_clicked() {
	///auto cmd = ui.command->text().toStdString();
	//ts_t newts {cmd};
	//std::vector<note_value> nvs {"1/1"_nv, "1/2"_nv, "1/4"_nv};
	//auto rrp = rand_rp(newts, nvs, std::vector<double>{1.0,1.0,1.0},0,bar_t{4});
	//std::string prp = printrp("4/4"_ts,*rrp);
	//gdp.create(&(*rrp),std::string("a random rp"));
	//ui.output->appendPlainText(QString::fromStdString(prp));
	wait();
}

void gui::on_actionImport_triggered() {
	//nf_import_window *nfimport_w = new nf_import_window(this);
	//nfimport_w->setAttribute(Qt::WA_DeleteOnClose);
	//nfimport_w->show();
}
