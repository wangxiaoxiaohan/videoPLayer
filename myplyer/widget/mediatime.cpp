#include "mediatime.h"
#include <QtDebug>
MediaTime::MediaTime(QWidget *parent) : QWidget(parent)
{
    QLabel *colon1 = new QLabel(this);
    QLabel *colon2 = new QLabel(this);

    int margin = 2;
    int textW = 10;
    int textH = 15;
    int colonW = 5;
    h1 = new QLabel(this);
    h1->setGeometry(margin,0,textW,textH);
    h2 = new QLabel(this);
    h2->setGeometry(margin * 2 + textW,0,textW,textH);
    h1->setAlignment(Qt::AlignCenter);
    h2->setAlignment(Qt::AlignCenter);

    colon1->setGeometry(margin * 3 + textW * 2 ,0,colonW,textH);
    colon1->setText(QString(":"));
    colon1->setAlignment(Qt::AlignCenter);

    min1 = new QLabel(this);
    min1->setGeometry(margin * 4 + textW * 2 + colonW,0,textW,textH);
    min2 = new QLabel(this);
    min2->setGeometry(margin * 5 + textW * 3 + colonW,0,textW,textH);
    min1->setAlignment(Qt::AlignCenter);
    min2->setAlignment(Qt::AlignCenter);


    colon2->setGeometry(margin * 6 + textW * 4 + colonW,0,colonW,textH);
    colon2->setText(QString(":"));
    colon2->setAlignment(Qt::AlignCenter);

    s1 = new QLabel(this);
    s1->setGeometry(margin * 7 + textW * 4 + colonW * 2,0,textW,textH);
    s2 = new QLabel(this);
    s2->setGeometry(margin * 8 + textW * 5 + colonW * 2,0,textW,textH);
    s1->setAlignment(Qt::AlignCenter);
    s2->setAlignment(Qt::AlignCenter);
}
void MediaTime::setTime(int second){
    int h = second / 3600 ;
    //qDebug() << "h" << h;
    h1->setText(QString::number(h / 10));
    h2->setText(QString::number(h % 10));

    int min = (second - (h * 3600)) / 60;
    //qDebug() << "min" << min;
    min1->setText(QString::number(min / 10));
    min2->setText(QString::number(min % 10));

    int s = (second - (h * 3600) - (min * 60));
    //qDebug() << "s" << s;
    s1->setText(QString::number(s / 10));
    s2->setText(QString::number(s % 10));
}
