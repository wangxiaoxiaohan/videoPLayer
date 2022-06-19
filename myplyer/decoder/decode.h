#ifndef DECODE_H
#define DECODE_H
extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
}
#include <QThread>
#include <QQueue>
#include <QDebug>
#include "base/framequeue.h"
#include "base/packetqueue.h"
class worker:public QObject
{
    Q_OBJECT
public:
    worker(packetqueue *pkt_q, framequeue *frame_q,AVCodecContext *dec_Ctx);
    ~worker();
private:
    framequeue *f_q;
    packetqueue *p_q;
    AVCodecContext *dec_ctx;
    bool quit_flag;
public slots:
    void work_thread();
    void quit_thread();
};

class decode:public QObject
{
    Q_OBJECT
public:
    decode();
    ~decode(){}
    int start(AVFormatContext *fmtCtx);
    void stop();
    int openCodec(int *stream_idx,
                  AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx, enum AVMediaType type);
    int decodePacket(AVCodecContext *avctx, AVPacket *packet,AVFrame *frame);
    enum AVPixelFormat getPixfmt();
    uchar* imageCopy(AVFrame *pframe);
public slots:
 //   int video_thread();
  //  int audio_thread();
  //  int subtitle_thread();
public:
    framequeue *video_frame_q;
    framequeue *audio_frame_q;
    framequeue *subtitle_frame_q;
    AVCodecContext *video_decCtx;
    AVCodecContext *audio_decCtx;
private:

    QThread* video_thread;
    QThread* audio_thread;
    worker *videoWorker;
    worker *audioWorker;
public:
    packetqueue *video_packq;
    packetqueue *audio_packq;
    packetqueue *subtitle_packq;
signals:
    void quitAudioThread();
    void quitVideoThread();
};

#endif // DECODE_H
