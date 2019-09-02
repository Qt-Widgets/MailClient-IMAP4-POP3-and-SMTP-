#ifndef _INDICATOR_BAR
#define _INDICATOR_BAR

#include <QtCore>
#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <QLabel>
#include <QListWidget>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMovie>

class IndicatorBar : public QLabel
{
    Q_OBJECT
public:
    IndicatorBar();

    void Start();
    void Stop();
private:
    QMovie movie;
};

#endif
