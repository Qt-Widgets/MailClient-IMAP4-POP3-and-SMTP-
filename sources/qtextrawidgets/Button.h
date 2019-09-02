#ifndef _BUTTON_
#define _BUTTON_

#include <QtCore>
#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <QLabel>
#include <QListWidget>
#include <QHBoxLayout>
#include <QLineEdit>


class Button : public QWidget
{
    Q_OBJECT
public:
    Button(QWidget* ptr = nullptr);
    Button(QString txt, QString fname, QWidget* ptr = nullptr);
    virtual ~Button();
    void setText(QString txt);
    void setImageFile(QString fname);

signals:
    void clicked();

protected:
    QSize sizeHint() const;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QString _Text;
    QPixmap _Image;
    bool    _ClickTrackingOn;
};

#endif
