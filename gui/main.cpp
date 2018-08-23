#include "gui2.h"
#include "data_pool.h"
#include "g_data_pool.h"
#include <QtWidgets/QApplication>


int main(int argc, char *argv[]) {
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication a(argc, argv);
	gui2 w;
	w.show();

	return a.exec();
}
