#pragma once

#include <QWidget>
#include "ui_fonts_colors.h"

class fonts_colors : public QWidget
{
	Q_OBJECT

public:
	fonts_colors(QWidget *parent = Q_NULLPTR);
	~fonts_colors();

private:
	Ui::fonts_colors ui;
};
