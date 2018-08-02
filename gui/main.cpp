#include "gui.h"
#include <QtWidgets/QApplication>
#include <vector>
#include "aulib\types\types_all.h"


int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	gui w;
	w.show();


	std::vector<note_value> nvs1 {"1/1"_nv, "1/2"_nv, "1/4."_nv, "1/4"_nv, "1/8"_nv};


	return a.exec();
}
