#include "VerticalLine.h"

VerticalLine::VerticalLine(int sz, QWidget* ptr) : QWidget(ptr)
{
    setMinimumWidth(1);
    setMaximumWidth(1);
    widgetHeight = sz;
}

VerticalLine::~VerticalLine()
{
}

void VerticalLine::SetHeight(int sz)
{
    widgetHeight = sz;
}

QSize VerticalLine::sizeHint() const
{
    int ht = 0;

    if(widgetHeight == -1)
    {
        ht = parentWidget()->height();
    }
    else
    {
        ht = widgetHeight;
    }

    return QSize(1, ht);
}

void VerticalLine::paintEvent(QPaintEvent *event)
{
    int ht = 0;

    if(widgetHeight == -1)
    {
        ht = parentWidget()->height();
    }
    else
    {
        ht = widgetHeight;
    }

    QPen penLine(QBrush(QColor(127,127,127)), 1);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(penLine);
    painter.drawLine(0, 0, 0, ht);
}
