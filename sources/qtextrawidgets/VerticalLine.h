#ifndef _VERTICAL_LINE
#define _VERTICAL_LINE

#include <QtCore>
#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <QLabel>
#include <QListWidget>
#include <QHBoxLayout>
#include <QLineEdit>

class VerticalLine : public QWidget
{
    Q_OBJECT
public:
    VerticalLine(int sz = -1, QWidget* ptr = nullptr);
    virtual ~VerticalLine();
    void SetHeight(int sz);
protected:
    QSize sizeHint() const;
    void paintEvent(QPaintEvent *event) override;
private:
    int widgetHeight;
};

#endif
