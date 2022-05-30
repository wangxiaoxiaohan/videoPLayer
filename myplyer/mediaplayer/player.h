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
public slots:
    void play_loop();
};
class splayer: public QObject{
    Q_OBJECT
public:
    splayer(glyuvwidget2* glw);
    ~splayer();
    void play();
    void pause();
    void stop();
    void seek();
    void setPlayRate();
    void prepare(QString file);
    void play_loop();
private:
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

friend class playThread;

};
#endif
