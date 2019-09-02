#ifndef _ACTIVE_LABEL
#define _ACTIVE_LABEL

#include <QtCore>
#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <QLabel>
#include <QListWidget>
#include <QHBoxLayout>
#include <QLineEdit>

class ActiveLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ActiveLabel( const QString& text="", QWidget* parent=0 );
    ~ActiveLabel();
signals:
    void clicked();
protected:
    void mousePressEvent(QMouseEvent* event);
};

#endif
