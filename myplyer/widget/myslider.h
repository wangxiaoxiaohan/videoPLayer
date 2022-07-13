#ifndef MYSLIDER_H
#define MYSLIDER_H

#include <QWidget>
#include <QSlider>
class mySlider : public QSlider
{
    Q_OBJECT
public:
    explicit mySlider(QWidget *parent = nullptr);
    bool pressed;
protected:
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
private:

signals:
    void releaseProgress(double value);
public slots:
};

#endif // MYSLIDER_H
