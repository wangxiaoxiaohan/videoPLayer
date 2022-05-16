#include "audiooutput.h"

audioOutput::audioOutput(QObject *parent) : QObject(parent)
{
    format.setCodec("audio/pcm");
    format.setSampleRate(48000);
    format.setSampleSize(32);
    format.setSampleType(QAudioFormat::Float);
    format.setChannelCount(2);
    format.setByteOrder(QAudioFormat::LittleEndian);
    output = new QAudioOutput(format, this);
    device = new audioDevice(this);
    device->open(QIODevice::ReadOnly);
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
    output->stop();
    device->refreshData(data,data.size());
    output->start(device);
}
