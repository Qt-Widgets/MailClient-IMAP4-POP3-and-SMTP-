#ifndef _HORIZONTAL_LINE
#define _HORIZONTAL_LINE

#include <QtCore>
#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <QLabel>
#include <QListWidget>
#include <QHBoxLayout>
#include <QLineEdit>

class HorizontalLine : public QWidget
{
    Q_OBJECT
public:
    HorizontalLine(int sz = -1, QWidget* ptr = nullptr);
    virtual ~HorizontalLine();
    void SetWidth(int sz);
protected:
    QSize sizeHint() const;
    void paintEvent(QPaintEvent *event) override;
private:
    int widgetWidth;
};

#endif
