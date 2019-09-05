#ifndef _RICH_DATA_LIST
#define _RICH_DATA_LIST

#include <QtCore>
#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <QLabel>
#include <QListWidget>
#include <QHBoxLayout>
#include <QLineEdit>

class RichDataListItemDelegate : public QAbstractItemDelegate
{
public:
	RichDataListItemDelegate(bool large = true, QObject *parent = 0);
    void paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
    QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    virtual ~RichDataListItemDelegate();
private:
    bool detailedRendering;
    int  rowHeight;
};

class RichDataList : public QListWidget
{
    Q_OBJECT
public:
	RichDataList(bool large);
    virtual ~RichDataList();
};

#endif
