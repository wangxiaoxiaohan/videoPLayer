#ifndef DEMUX_H
#define DEMUX_H
#include <QSharedPointer>
#include <QThread>
#include <QQueue>
extern "C"{
#include <libavformat/avformat.h>
}
#include <base/packetqueue.h>

class demux : public QObject
{
    Q_OBJECT
public:
    demux();
    int srart(QString uri);
    int openFile(QString path);
private:
    int findStreamInfo();


    int subtitle_steam_index;
public slots:
    int read_thread();
signals:

public:

    int audio_steam_index;
    int video_steam_index;
    packetqueue *video_packq;
    packetqueue *audio_packq;
    packetqueue *subtitle_packq;
    AVFormatContext *fmtCtx;
signals:
    void finished();
    void error(QString err);
};

#endif // DEMUX_H
