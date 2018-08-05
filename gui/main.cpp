#include "gui.h"
#include "gui2.h"
#include "fonts_colors.h"
#include "nf_import_window.h"
#include "aulib\types\types_all.h"
#include "aulib\util\au_util_all.h"
#include "data_pool.h"
#include "g_data_pool.h"
#include <QtWidgets/QApplication>
#include <vector>
#include <string>


int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	gui w;
	w.show();

	//std::vector<note_value> nvs1 {"1/1"_nv, "1/2"_nv, "1/4."_nv, "1/4"_nv, "1/8"_nv};

	//nf_import_window *nfimport_w = new nf_import_window();
	//nfimport_w->setAttribute(Qt::WA_DeleteOnClose);
	//nfimport_w->show();

	gui2 *g2w = new gui2();
	g2w->setAttribute(Qt::WA_DeleteOnClose);
	g2w->show();


	wait();
	return a.exec();
}
