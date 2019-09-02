#ifndef _DATA_LIST
#define _DATA_LIST

#include <QtCore>
#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <QLabel>
#include <QListWidget>
#include <QHBoxLayout>
#include <QLineEdit>

class DataListItemDelegate : public QAbstractItemDelegate
{
public:
    DataListItemDelegate(QObject *parent = 0);
    void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
    QSize sizeHint (const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    virtual ~DataListItemDelegate();
};

class DataList : public QListWidget
{
    Q_OBJECT
public:
    DataList();
    virtual ~DataList();
};

#endif
