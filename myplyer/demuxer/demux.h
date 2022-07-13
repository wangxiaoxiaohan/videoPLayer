#ifndef DEMUX_H
#define DEMUX_H
#include <QSharedPointer>
#include <QThread>
#include <QQueue>
#include <QMutex>
extern "C"{
#include <libavformat/avformat.h>
}
#include <base/packetqueue.h>
class demux;
class demuxWorker:public QObject
{
    Q_OBJECT
public:
    demuxWorker(demux *demuxer);
    ~demuxWorker();

public slots:
    void work_thread();
    void quit_threag_flag();
private:
    demux *mDemuxer;
    bool quit_flag;
};
class demux : public QObject
{
    Q_OBJECT
public:
    demux();
    int start(QString file);
    int stop();
    int openFile(QString path);
    int seek(double timeStamp);
private:
    int findStreamInfo();
    QThread* demuxThread;
    demuxWorker *worker;
    QString url;
    bool seek_flag;
    double mTimeStamp;
public slots:

signals:
    void quitDemuxThread();
public:

    int audio_steam_index;
    int video_steam_index;
    packetqueue *video_packq;
    packetqueue *audio_packq;
    packetqueue *subtitle_packq;
    AVFormatContext *fmtCtx;
    QMutex *mutex;

signals:
    void finished();
    void error(QString err);
friend class demuxWorker;
};

#endif // DEMUX_H
