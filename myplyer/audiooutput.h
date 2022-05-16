#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

#include <QObject>
#include <QAudioOutput>
#include <QTimer>
#include "audiodevice.h"
class audioOutput : public QObject
{
    Q_OBJECT
public:
    explicit audioOutput(QObject *parent = nullptr);

signals:

public slots:
    void playRawAudio(QByteArray data);
public:
    void setAudioFormat(QString codec,int smapleRate,int sampleSize,int channelCount,
                        QAudioFormat::SampleType sampleType,QAudioFormat::Endian byteOrder);
private:
    QAudioFormat format;
    QAudioOutput *output;
    audioDevice *device;
};

#endif // AUDIOOUTPUT_H
