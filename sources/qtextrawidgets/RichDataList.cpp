#include "RichDataList.h"
#include "ThemeHandler.h"

RichDataListItemDelegate::RichDataListItemDelegate(bool large, QObject *parent)
{
    _Large = large;

    if(_Large)
    {
        _Height = 60;
    }
    else
    {
        _Height = 40;
    }
}

void RichDataListItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    if(_Large)
    {
        paintLarge(painter, option, index);
    }
    else
    {
        paintSmall(painter, option, index);
    }
}

void RichDataListItemDelegate::paintLarge(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    QIcon ic = QIcon(qvariant_cast<QPixmap>(index.data(Qt::DecorationRole)));
    QString title = index.data(Qt::DisplayRole).toString();
    QString description = index.data(Qt::UserRole).toString();
    int imageSpace = 10;
	QRect r = option.rect;

    QPen penTextNormal(currentThemePalette().text(), 1, Qt::SolidLine);
    QPen penTextSelected(currentThemePalette().highlight(), 1, Qt::SolidLine);

    QFont fontHeadingNormal(currentFont(), 10, QFont::Normal);
    QFont fontDescriptionNormal(currentFont(), 9, QFont::Normal);
    QFont fontHeadingHighlighted(currentFont(), 10, QFont::Bold);
    QFont fontDescriptionHighlighted(currentFont(), 9, QFont::Bold);

    if(option.state & QStyle::State_Selected)
    {
        painter->setBrush(currentThemePalette().highlight());
    }
    else
    {
        painter->setBrush(currentThemePalette().window());
    }

    painter->setPen(penTextSelected);

    if(option.state & QStyle::State_Selected)
    {
        painter->setPen(penTextSelected);
    }
    else
    {
        painter->setPen(penTextNormal);
    }

    if (!ic.isNull())
    {
        r = option.rect.adjusted(5, 10, -10, -10);
        ic.paint(painter, r, Qt::AlignVCenter|Qt::AlignLeft);
        imageSpace = 55;
    }

    r = option.rect.adjusted(imageSpace, 0, -10, -30);

    if(option.state & QStyle::State_Selected)
    {
        painter->setFont(fontHeadingHighlighted);
    }
    else
    {
        painter->setFont(fontHeadingNormal);
    }

    painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignBottom|Qt::AlignLeft, title, &r);

    r = option.rect.adjusted(imageSpace, 30, -10, 0);

    if(option.state & QStyle::State_Selected)
    {
        painter->setFont(fontDescriptionHighlighted);
    }
    else
    {
        painter->setFont(fontDescriptionNormal);
    }

    painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignLeft, description, &r);
}

void RichDataListItemDelegate::paintSmall(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    QIcon ic = QIcon(qvariant_cast<QPixmap>(index.data(Qt::DecorationRole)));
    QString title = index.data(Qt::DisplayRole).toString();
    QString description = index.data(Qt::UserRole).toString();
    QRect r = option.rect;

    QPen penTextNormal(currentThemePalette().text(), 1, Qt::SolidLine);
    QPen penTextSelected(currentThemePalette().highlight(), 1, Qt::SolidLine);

    QFont fontHeadingNormal(currentFont(), 9, QFont::Normal);
    QFont fontHeadingHighlighted(currentFont(), 8, QFont::Bold);

    if(option.state & QStyle::State_Selected)
    {
        painter->setBrush(currentThemePalette().highlight());
    }
    else
    {
        painter->setBrush(currentThemePalette().window());
    }

    painter->setPen(penTextSelected);

    if(option.state & QStyle::State_Selected)
    {
        painter->setPen(penTextSelected);
    }
    else
    {
        painter->setPen(penTextNormal);
    }

    if(option.state & QStyle::State_Selected)
    {
        painter->setFont(fontHeadingHighlighted);
    }
    else
    {
        painter->setFont(fontHeadingNormal);
    }

    painter->drawText(r.left() + 55 , r.top(), r.width(), r.height(), Qt::AlignVCenter|Qt::AlignLeft, title, &r);
}

QSize RichDataListItemDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    return QSize(200, _Height);
}

RichDataListItemDelegate::~RichDataListItemDelegate()
{

}

RichDataList::RichDataList(bool large)
{
    setItemDelegate(new RichDataListItemDelegate(large, this));
}

RichDataList::~RichDataList()
{

}
