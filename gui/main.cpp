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
	gui2 w;
	w.show();

	return a.exec();
}
