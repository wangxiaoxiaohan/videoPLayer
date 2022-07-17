#ifndef PALYER_H
#define PALYER_H
#include "glyuvwidget3.h"
#include "audiooutput.h"
#include "demuxer/demux.h"
#include "decoder/decode.h"
class splayer;
class playThread:public QObject
{
    Q_OBJECT
public:
    playThread(splayer *player,AVMediaType type);
    ~playThread(){}
private:
    AVMediaType mMediaType;
    splayer* mPlayer;
signals:
    void signalShowYuv(uchar *ptr_y,uchar *ptr_u,uchar *ptr_v,uint width, uint height);
public slots:
    void play_loop();
};
enum mediaStatus{
    media_idle = 0,
    media_stopped,
    media_playing,
    media_paused,
    media_seeking,
};
class splayer: public QObject{
    Q_OBJECT
public:
    splayer(glyuvwidget2* glw);
    ~splayer();
    void play();
    void pause();
    void stop();
    void seek(double precent);
    void setPlayRate(double speed);
    double playRate();
    void prepare(QString file);
    void play_loop();
    void reset();
    enum mediaStatus mediaStatus();
signals:
    void updateProgress(int current,int total);
    void vSync();
public slots:
    void vSyncSlot();
private:
    void stopDecoderDemuxer();
    void stopState();
    glyuvwidget2* glwidget;
    audioOutput* audioOut;
    demux* demuxer;
    decode* decoder;
    AVFormatContext *fmt_ctx;
    QWidget *parent;
    double audio_clock;
    double video_clock;
    AVRational audio_timebase;
    AVRational video_timebase;
    enum mediaStatus mMediaStatus;
    QWaitCondition condition;
    QMutex mutex;
   // QThread* demuxThread;
    QThread* audio_th;
    QThread* video_th;
    double playSpeed;

    //uchar* y_ptr;
    //uchar* u_ptr;
    //uchar* v_ptr;

    uchar* y_ptr_front;
    uchar* u_ptr_front;
    uchar* v_ptr_front;

    uchar* y_ptr_back;
    uchar* u_ptr_back;
    uchar* v_ptr_back;

    int bufSize_y;
    int bufSize_u;
    int bufSize_v;
friend class playThread;

};
#endif
