#include "HorizontalLine.h"

HorizontalLine::HorizontalLine(int sz, QWidget* ptr) : QWidget(ptr)
{
    setMinimumHeight(1);
    setMaximumHeight(1);
    widgetWidth = sz;
}

HorizontalLine::~HorizontalLine()
{
}

void HorizontalLine::SetWidth(int sz)
{
    widgetWidth = sz;
}

QSize HorizontalLine::sizeHint() const
{
    int wd = 0;

    if(widgetWidth == -1)
    {
        wd = parentWidget()->width();
    }
    else
    {
        wd = widgetWidth;
    }

    return QSize(wd, 1);
}

void HorizontalLine::paintEvent(QPaintEvent *event)
{
    int wd = 0;

    if(widgetWidth == -1)
    {
        wd = parentWidget()->width();
    }
    else
    {
        wd = widgetWidth;
    }

    QPen penLine(QBrush(QColor(127,127,127)), 1);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(penLine);
    painter.drawLine(0, 0, wd, 0);
}

