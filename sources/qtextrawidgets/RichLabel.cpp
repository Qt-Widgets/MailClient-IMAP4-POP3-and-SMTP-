#include "RichLabel.h"
#include "ThemeHandler.h"

RichLabel::RichLabel(QWidget* ptr) : QWidget(ptr)
{
    _UnderLine = true;
    _ClickTrackingOn = false;
    _Large = true;
    _Bold = false;
    _Height = 60;

    setMaximumHeight(_Height);
    setMinimumHeight(_Height);
}

RichLabel::RichLabel(QString txt, QString fname, QWidget* ptr) : QWidget(ptr)
{
    _UnderLine = true;
    _ClickTrackingOn = false;
    _Large = true;
    _Bold = false;
    _Height = 60;

    setMaximumHeight(_Height);
    setMinimumHeight(_Height);

    setText(txt);
    setImageFile(fname);
}

RichLabel::~RichLabel()
{
}

void RichLabel::setText(QString txt)
{
    _Text = txt;
}

void RichLabel::setImageFile(QString fname)
{
    _Image = QPixmap(fname).scaled(32, 32);
}

void RichLabel::setUnderline(bool fl)
{
    _UnderLine = fl;
}

void RichLabel::setLarge(bool largef)
{
    _Large = largef;

    if(_Large)
    {
        _Height = 60;
    }
    else
    {
        _Height = 40;
    }
}

void RichLabel::setBold(bool boldf)
{
    _Bold = boldf;
}

QSize RichLabel::sizeHint() const
{
    int wd = parentWidget()->width();

    return QSize(wd, _Height);
}

void RichLabel::paintEvent(QPaintEvent *event)
{
    int wd = parentWidget()->width();
    QRect rcb(_Height, 0, 190, _Height);
    QRect rc(0, 0, wd, _Height);

    QFont fontHeadingHighlighted(currentFont(), 12, QFont::Normal);
    QPen penText(QBrush(currentThemePalette().text()), 1);
    QPen penBorder(QBrush(currentThemePalette().base()), 1);
    QPen penLine(QBrush(QColor(127,127,127)), 1);

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
        if(_Large)
        {
            painter.drawPixmap(14, 14, 32, 32, _Image);
        }
    }

    if(_UnderLine)
    {
        painter.setPen(penLine);
        painter.drawLine(0, _Height, wd, _Height);
    }
}

void RichLabel::mousePressEvent(QMouseEvent *event)
{
    _ClickTrackingOn = true;
}

void RichLabel::mouseReleaseEvent(QMouseEvent *event)
{
    if(_ClickTrackingOn)
    {
        emit clicked();
        _ClickTrackingOn = false;
    }
}
