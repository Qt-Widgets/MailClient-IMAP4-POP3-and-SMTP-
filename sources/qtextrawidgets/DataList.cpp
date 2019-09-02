#include "DataList.h"
#include "ThemeHandler.h"

DataListItemDelegate::DataListItemDelegate(QObject *parent)
{

}

void DataListItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    bool imagedrawn = false;

    QString title = index.data(Qt::DisplayRole).toString();
    QString description = index.data(Qt::UserRole).toString();
    int imageSpace = 10;
    QRect r = option.rect;

    QPen penTextNormal(currentThemePalette().text(), 1, Qt::SolidLine);
    QPen penTextSelected(currentThemePalette().highlight(), 1, Qt::SolidLine);

    QFont fontHeadingNormal(currentFont(), 10, QFont::Normal);
    QFont fontDescriptionNormal(currentFont(), 8, QFont::Normal);
    QFont fontHeadingHighlighted(currentFont(), 10, QFont::Normal);
    QFont fontDescriptionHighlighted(currentFont(), 8, QFont::Normal);

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

    QPixmap pix = qvariant_cast<QPixmap>(index.data(Qt::DecorationRole));
    if(!pix.isNull())
    {
        QIcon ic = QIcon(pix.scaled(42, 42));
        if (!ic.isNull())
        {
            QRect rpix = r;
            rpix.setX(rpix.x()+9);
            ic.paint(painter, rpix, Qt::AlignVCenter|Qt::AlignLeft);
            imageSpace = 55;
            imagedrawn = true;
        }
    }

    if(imagedrawn)
    {
        r = option.rect.adjusted(imageSpace, 0, -10, -30);
    }
    else
    {
        r = option.rect.adjusted(10, 5, 0, 0);
    }

    if(option.state & QStyle::State_Selected)
    {
        painter->setFont(fontHeadingHighlighted);
    }
    else
    {
        painter->setFont(fontHeadingNormal);
    }

    painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignTop|Qt::AlignLeft, title, &r);

    if(imagedrawn)
    {
        r = option.rect.adjusted(imageSpace, 30, -10, 0);
    }
    else
    {
        r = option.rect.adjusted(10, 20, 0, 0);
    }

    if(option.state & QStyle::State_Selected)
    {
        painter->setFont(fontDescriptionHighlighted);
    }
    else
    {
        painter->setFont(fontDescriptionNormal);
    }

    painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignBottom|Qt::AlignLeft, description, &r);
}

QSize DataListItemDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    return QSize(200, 40);
}

DataListItemDelegate::~DataListItemDelegate()
{

}

DataList::DataList()
{
    setItemDelegate(new DataListItemDelegate(this));
}

DataList::~DataList()
{

}
