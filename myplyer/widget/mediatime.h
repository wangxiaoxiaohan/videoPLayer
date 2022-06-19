#ifndef MEDIATIME_H
#define MEDIATIME_H

#include <QWidget>
#include <QLabel>

class MediaTime : public QWidget
{
    Q_OBJECT
public:
    explicit MediaTime(QWidget *parent = nullptr);
    void setTime(int second);
signals:

public slots:

private:
    QLabel *h1;
    QLabel *h2;
    QLabel *min1;
    QLabel *min2;
    QLabel *s1;
    QLabel *s2;
};

#endif // MEDIATIME_H
