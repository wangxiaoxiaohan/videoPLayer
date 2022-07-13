#include "myslider.h"

mySlider::mySlider(QWidget *parent) : QSlider(parent)
{
    pressed = false;
}
void mySlider::mousePressEvent(QMouseEvent *ev){
    pressed = true;
    QSlider::mousePressEvent(ev);
}
void mySlider::mouseReleaseEvent(QMouseEvent *ev){
    pressed = false;
    QSlider::mouseReleaseEvent(ev);
    //value();
    emit releaseProgress(value());
}
void mySlider::mouseMoveEvent(QMouseEvent *ev){

   QSlider::mouseMoveEvent(ev);
}
