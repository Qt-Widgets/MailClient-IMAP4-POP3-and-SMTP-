#ifndef _RICH_LABEL
#define _RICH_LABEL

#include <QtCore>
#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <QLabel>
#include <QListWidget>
#include <QHBoxLayout>
#include <QLineEdit>

class RichLabel : public QWidget
{
    Q_OBJECT
public:
    RichLabel(QWidget* ptr = nullptr);
    RichLabel(QString txt, QString fname, QWidget* ptr = nullptr);
    virtual ~RichLabel();
    void setText(QString txt);
    void setImageFile(QString fname);
    void setUnderline(bool fl = true);
    void setLarge(bool largef);
    void setBold(bool boldf);

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
    bool    _UnderLine;
    bool    _ClickTrackingOn;
    bool    _Large;
    int     _Height;
    bool    _Bold;
};

#endif
