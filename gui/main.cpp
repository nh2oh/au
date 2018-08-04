#include "gui.h"
#include "fonts_colors.h"
#include <QtWidgets/QApplication>
#include <vector>
#include "aulib\types\types_all.h"
#include "aulib\util\au_util_all.h"
#include <string>

//#include "au_slots.h"

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	//gui w;
	gui w(std::string("special constructor!!"));
	w.show();
	
	fonts_colors fc;
	fc.show();

	//std::vector<note_value> nvs1 {"1/1"_nv, "1/2"_nv, "1/4."_nv, "1/4"_nv, "1/8"_nv};

	wait();
	return a.exec();
}
