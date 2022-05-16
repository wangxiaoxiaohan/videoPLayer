#ifndef PALYER_H
#define PALYER_H
#include "glyuvwidget3.h"
#include "audiooutput.h"
#include "demuxer/demux.h"
#include "decoder/decode.h"

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

};
#endif
