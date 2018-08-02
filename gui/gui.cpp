#include "gui.h"
#include "aulib\rpgen\rand_rp.h"
#include "aulib\types\types_all.h"
#include "aulib\util\au_util_all.h"
#include <string>
#include <optional>
#include <vector>

gui::gui(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}

gui::gui(std::string str_in, QWidget *parent) : QMainWindow(parent) {
	m_str = str_in;
	ui.setupUi(this);
}

void gui::on_btn_make_clicked() {
	auto cmd = ui.command->text().toStdString();
	ts_t newts {cmd};
	std::vector<note_value> nvs {"1/1"_nv, "1/2"_nv, "1/4"_nv};
	auto rrp = rand_rp(newts, nvs, std::vector<double>{1.0,1.0,1.0},0,bar_t{4});
	std::string prp = printrp("4/4"_ts,*rrp);
	ui.output->appendPlainText(QString::fromStdString(prp));
	ui.output->appendPlainText(QString::fromStdString(m_str));
	wait();
}

