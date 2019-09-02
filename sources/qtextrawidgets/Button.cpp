#include "Button.h"
#include "ThemeHandler.h"

Button::Button(QWidget* ptr) : QWidget(ptr)
{
    setMaximumHeight(60);
    setMinimumHeight(60);
    setMinimumWidth(250);
    setMaximumWidth(250);
    _ClickTrackingOn = false;
}

Button::Button(QString txt, QString fname, QWidget* ptr) : QWidget(ptr)
{
    setMaximumHeight(60);
    setMinimumHeight(60);
    setMinimumWidth(250);
    setMaximumWidth(250);

    setText(txt);
    setImageFile(fname);
    _ClickTrackingOn = false;
}

Button::~Button()
{
}

void Button::setText(QString txt)
{
    _Text = txt;
}

void Button::setImageFile(QString fname)
{
    _Image = QPixmap(fname).scaled(32, 32);
}

QSize Button::sizeHint() const
{
    return QSize(250, 60);
}

void Button::paintEvent(QPaintEvent *event)
{
    QRect rcb(60, 0, 190, 60);
    QRect rc(0, 0, 250, 60);

    QFont fontHeadingHighlighted(currentFont(), 12, QFont::Normal);
    QPen penText(QBrush(currentThemePalette().text()), 1);
    QPen penBorder(QBrush(currentThemePalette().base()), 1);
    QPen penLine(QBrush(currentThemePalette().alternateBase()), 1);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(penBorder);
    painter.setBrush(currentThemePalette().base());
    painter.drawRect(rcb);

    painter.setPen(penText);
    painter.setFont(fontHeadingHighlighted);

    if(_Text.length()> 0)
    {
        painter.drawText(rc.left() + 65 , rc.top(), rc.width()-65, rc.height(), Qt::AlignVCenter|Qt::AlignLeft, _Text, &rc);
    }

    if(!_Image.isNull())
    {
        painter.drawPixmap(14, 14, 32, 32, _Image);
    }
}

void Button::mousePressEvent(QMouseEvent *event)
{
    _ClickTrackingOn = true;
}

void Button::mouseReleaseEvent(QMouseEvent *event)
{
    if(_ClickTrackingOn)
    {
        emit clicked();
        _ClickTrackingOn = false;
    }
}
