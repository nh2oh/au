#include "gui2.h"
#include "nf_import_window.h"
#include "g_data_pool.h"

gui2::gui2(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);


}

void gui2::on_actionImport_triggered() {
	nf_import_window *nfimport_w = new nf_import_window(this);
	nfimport_w->setAttribute(Qt::WA_DeleteOnClose);
	nfimport_w->show();
}
