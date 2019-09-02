#include "ActiveLabel.h"
#include "ThemeHandler.h"

ActiveLabel::ActiveLabel(const QString& text, QWidget* parent)
    : QLabel(parent)
{
    setText(text);
}

ActiveLabel::~ActiveLabel()
{
}

void ActiveLabel::mousePressEvent(QMouseEvent* event)
{
    emit clicked();
}
