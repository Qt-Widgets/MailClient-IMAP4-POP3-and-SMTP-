#ifndef _PICTURE_BOX
#define _PICTURE_BOX

#include <QLabel>

class PictureBox : public QLabel
{
Q_OBJECT
public:
    explicit PictureBox( const QString& text="", QWidget* parent=0 );
    ~PictureBox();
signals:
    void clicked();
protected:
    void mousePressEvent(QMouseEvent* event);
};

#endif
