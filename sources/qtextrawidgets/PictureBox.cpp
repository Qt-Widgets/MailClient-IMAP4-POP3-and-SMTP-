#include "PictureBox.h"

PictureBox::PictureBox(const QString& text, QWidget* parent)
    : QLabel(parent)
{
    setText(text);
}

PictureBox::~PictureBox()
{
}

void PictureBox::mousePressEvent(QMouseEvent* event)
{   (void)event;
    emit clicked();
}
