#include "audiooutput.h"
#include <QDebug>
audioOutput::audioOutput(QObject *parent) : QObject(parent)
{
    format.setCodec("audio/pcm");
    format.setSampleRate(48000);
    format.setSampleSize(32);
    format.setSampleType(QAudioFormat::Float);
    format.setChannelCount(1);
    format.setByteOrder(QAudioFormat::LittleEndian);
    output = new QAudioOutput(format, this);
    output->setVolume(100);

    device = output->start();

 //   device = new audioDevice(this);
 //   device->open(QIODevice::ReadOnly);
//    sourceFile.setFileName("/home/wanghan/test/qpcm.pcm");
//    sourceFile.open(QIODevice::ReadOnly);
}
void audioOutput::setAudioFormat(QString codec,int smapleRate,int sampleSize,int channelCount,
                    QAudioFormat::SampleType sampleType,QAudioFormat::Endian byteOrder)
{
    format.setCodec(codec);
    format.setSampleRate(smapleRate);
    format.setSampleSize(sampleSize);
    format.setSampleType(sampleType);
    format.setChannelCount(channelCount);
    format.setByteOrder(byteOrder);

}
void audioOutput::playRawAudio(QByteArray data){
   // qDebug() << "data size" << data.size();
    device->write(data.data(),data.length());
}
