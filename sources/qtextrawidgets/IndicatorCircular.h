#ifndef _INDICATOR_CIRCULAR
#define _INDICATOR_CIRCULAR

#include <QtCore>
#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <QLabel>
#include <QListWidget>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMovie>

class IndicatorCircular : public QLabel
{
    Q_OBJECT
public:
    IndicatorCircular();

    void Start();
    void Stop();
private:
    QMovie movie;
};

#endif
