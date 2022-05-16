#ifndef AUDIODEVICE_H
#define AUDIODEVICE_H

#include <QIODevice>
class audioDevice : public QIODevice
{
    Q_OBJECT
public:
    audioDevice(QObject *parent);
    ~audioDevice(){}
    virtual qint64 readData(char *data, qint64 maxlen);
    virtual qint64 writeData(const char *data, qint64 len);
    void refreshData(QByteArray data,int size);

signals:
private:
    QByteArray mData;
    int dataSize;
public slots:
};

#endif // AUDIODEVICE_H
