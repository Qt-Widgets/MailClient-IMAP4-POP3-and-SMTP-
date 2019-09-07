#include "RichDataList.h"
#include "ThemeHandler.h"

RichDataListItemDelegate::RichDataListItemDelegate(bool large, QObject *parent)
{
    detailedRendering = large;

    if(detailedRendering)
    {
        rowHeight = 60;
    }
    else
    {
        rowHeight = 40;
    }
}

void RichDataListItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	QIcon ic = QIcon(qvariant_cast<QPixmap>(index.data(Qt::DecorationRole)));
	QString title = index.data(Qt::DisplayRole).toString();
	QString description = index.data(Qt::UserRole).toString();
	bool bolden_normal = index.data(Qt::UserRole + 2).toBool();

	int imageSpace = 0;

	if (detailedRendering)
	{
		imageSpace = 10;
	}
	else
	{
		imageSpace = 5;
	}

	QRect r = option.rect;

	QRect widget_rect = option.rect;

	QPen penText(currentThemePalette().text(), 1, Qt::SolidLine);
	QPen penItemSeparator(currentThemePalette().text(), 1, Qt::DotLine);

	QFont fontHeadingNormal(currentFont(), 10, QFont::Normal);
	QFont fontDescriptionNormal(currentFont(), 9, QFont::Normal);
	QFont fontHeadingBold(currentFont(), 10, QFont::Bold);
	QFont fontDescriptionBold(currentFont(), 9, QFont::Bold);

	if (option.state & QStyle::State_Selected)
	{

		painter->fillRect(widget_rect, currentThemePalette().highlight());
	}
	else
	{
		painter->fillRect(widget_rect, currentThemePalette().window());
	}

	if (!ic.isNull())
	{
		if (detailedRendering)
		{
			QRect image_rect = option.rect.adjusted(5, 10, -10, -10);
			ic.paint(painter, image_rect, Qt::AlignVCenter | Qt::AlignLeft);

			imageSpace = 55;
		}
		else
		{
			QRect image_rect = option.rect.adjusted(5, 5, -10, -5);
			ic.paint(painter, image_rect, Qt::AlignVCenter | Qt::AlignLeft);

			imageSpace = 40;
		}
	}


	if (bolden_normal && !(option.state & QStyle::State_Selected))
	{
		painter->setFont(fontHeadingNormal);
	}
	else
	{
		painter->setFont(fontHeadingBold);
	}

	if (detailedRendering)
	{
		r = option.rect.adjusted(imageSpace, 0, -10, -30);
		painter->setPen(penText);
		painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignBottom | Qt::AlignLeft, title, &r);
	}
	else
	{
		r = option.rect.adjusted(imageSpace, 10, -10, -15);
		painter->setPen(penText);
		painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignVCenter | Qt::AlignLeft, title, &r);
	}

	r = option.rect.adjusted(imageSpace, 30, -10, 0);

	if (bolden_normal && !(option.state & QStyle::State_Selected))
	{
		painter->setFont(fontDescriptionNormal);
	}
	else
	{
		painter->setFont(fontDescriptionBold);
	}

	if (detailedRendering)
	{
		painter->setPen(penText);
		painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignLeft, description, &r);
	}

	painter->setPen(penItemSeparator);
	painter->drawLine(widget_rect.left(), widget_rect.bottom(), widget_rect.left() + widget_rect.width(), widget_rect.bottom());
}

QSize RichDataListItemDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    return QSize(200, rowHeight);
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
