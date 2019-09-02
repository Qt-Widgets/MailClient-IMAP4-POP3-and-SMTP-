#ifndef _CENTRAL_WIDGET
#define _CENTRAL_WIDGET

#include <QtCore>
#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <QLabel>
#include <QListWidget>
#include <QHBoxLayout>
#include <QLineEdit>

class CentralWidget : public QWidget
{
    Q_OBJECT
public:
    CentralWidget(QWidget *parent = nullptr);
    ~CentralWidget();
    void addWidget(QWidget *widget);
private:
    QHBoxLayout _Layout;
};

#endif