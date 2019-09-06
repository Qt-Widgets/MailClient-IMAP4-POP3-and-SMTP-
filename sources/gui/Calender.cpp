#include "Calender.h"

Calender::Calender(QWidget *parent) : QWidget(parent)
{
	layout.addWidget(&calender, 0, 0, 1, 1);
	calender.setGridVisible(true);
	setLayout(&layout);
}
