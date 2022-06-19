 #include "decoder/decode.h"
#include <QDebug>
#include <QCoreApplication>
worker::worker(packetqueue *pkt_q, framequeue *frame_q,AVCodecContext *dec_Ctx){
        p_q = pkt_q;
        f_q = frame_q;
        dec_ctx = dec_Ctx;
        quit_flag = false;
        qDebug() << "decode worker struct" << dec_ctx->codec_type;
}
void worker::quit_thread(){
    qDebug() << "decode receive quit signal";
    quit_flag = true;
}
worker::~worker(){
    qDebug() << "decode worker destruct";
}
void worker::work_thread(){
        int ret;
        AVFrame *frame = av_frame_alloc();

        while(!quit_flag){
            qDebug() << "decoding!!!"<< dec_ctx->codec_type << QThread::currentThreadId();
            while(p_q->size() == 0){
               // qDebug() << "decoding!!!00000000"<< dec_ctx->codec_type << QThread::currentThreadId();
               QThread::usleep(20000);
               QCoreApplication::processEvents(QEventLoop::AllEvents,100);
            }
           // qDebug() << "decoding!!!11111111"<< dec_ctx->codec_type << QThread::currentThreadId();
           AVPacket pkt;
           p_q->get(&pkt);
           // qDebug() << "decoding!!!2222222222"<< dec_ctx->codec_type << QThread::currentThreadId();
           ret = avcodec_send_packet(dec_ctx, &pkt);
           //qDebug() << "avcodec_send_packet ret" << ret;
           if(ret < 0){
                qDebug() << "avcodec_send_packet error ret" << ret;
               exit(1);
           }
           av_packet_unref(&pkt);
           // must try to avcodec_receive_frame several times
           while(ret >= 0){
              // qDebug() << "decoding!!!2222222222"<< dec_ctx->codec_type << QThread::currentThreadId();
                ret = avcodec_receive_frame(dec_ctx, frame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                    break;
                else if(ret < 0){
                    qDebug() << "receive frame error!";
                    exit(0);
                }
                //qDebug() << "decoding!!!333333"<< dec_ctx->codec_type << QThread::currentThreadId();
                Frame *f = f_q->getEmptyFrame();
                f->pts = frame->pts;
                av_frame_move_ref(f->af, frame);
                av_frame_unref(frame);
                //qDebug() << "decoding!!!44444444"<< dec_ctx->codec_type << QThread::currentThreadId();
                f_q->put();
               // qDebug() << "decoding!!!555555555"<< dec_ctx->codec_type << QThread::currentThreadId();
                QCoreApplication::processEvents(QEventLoop::AllEvents,100);
           }
            QCoreApplication::processEvents(QEventLoop::AllEvents,100);
            qDebug() << "decoding!!!6666666666666"<< dec_ctx->codec_type << QThread::currentThreadId();
        }
        qDebug() << "decode real quit  threading!!!!!!!!!!! type" << dec_ctx->codec_type << QThread::currentThreadId();
        av_frame_free(&frame);
        avcodec_free_context(&dec_ctx);
        f_q->clear();

}
decode::decode()
{

    video_frame_q = new framequeue();
    audio_frame_q = new framequeue();
    subtitle_frame_q = new framequeue();

}

enum AVPixelFormat decode::getPixfmt(){
    return video_decCtx->pix_fmt;
}
int decode::openCodec(int *stream_idx,
                      AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx, enum AVMediaType type){
    int ret, stream_index;
    AVStream *st;
    AVCodec *dec = NULL;
    AVDictionary *opts = NULL;
    ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
    if (ret < 0) {
        qDebug() <<  "Could not find %s stream in input file ";
        return ret;
    } else {
        stream_index = ret;
        st = fmt_ctx->streams[stream_index];

        /* Allocate a codec context for the decoder */
        *dec_ctx = avcodec_alloc_context3(dec);
        if (!*dec_ctx) {
            fprintf(stderr, "Failed to allocate the %s codec context\n",
                    av_get_media_type_string(type));
            return AVERROR(ENOMEM);
        }
        qDebug() <<  "openCodec 5555555";
        /* Copy codec parameters from input stream to output codec context */
        if ((ret = avcodec_parameters_to_context(*dec_ctx, st->codecpar)) < 0) {
            fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n",
                    av_get_media_type_string(type));
            return ret;
        }

        /* find decoder for the stream */
        dec = avcodec_find_decoder(st->codecpar->codec_id);
        if (!dec) {
            fprintf(stderr, "Failed to find %s codec\n",
                    av_get_media_type_string(type));
            return AVERROR(EINVAL);
        }

        /* Init the decoders, with or without reference counting */
        av_dict_set(&opts, "refcounted_frames","1", 0);
        if ((ret = avcodec_open2(*dec_ctx, dec, &opts)) < 0) {
            fprintf(stderr, "Failed to open %s codec\n",
                    av_get_media_type_string(type));
            return ret;
        }
        *stream_idx = stream_index;

    }

}
int decode::start(AVFormatContext *fmtCtx){

    video_frame_q->clear();
    audio_frame_q->clear();

    int ret ;
    qDebug() << "decode::start";
    int audioStreamIdx = 0;
    int videoStreamIdx = 0;
    ret = openCodec(&videoStreamIdx,&video_decCtx,fmtCtx,AVMEDIA_TYPE_VIDEO);
    qDebug() << "openCodec: video ret " << ret;


    ret = openCodec(&audioStreamIdx,&audio_decCtx,fmtCtx,AVMEDIA_TYPE_AUDIO);
    qDebug() << "openCodec: audio ret " << ret;


    video_thread = new QThread;
    videoWorker = new worker(video_packq,video_frame_q,video_decCtx);
    videoWorker->moveToThread(video_thread);

    connect(video_thread, SIGNAL(started()), videoWorker, SLOT(work_thread()));
    connect(this, SIGNAL(quitVideoThread()), videoWorker, SLOT(quit_thread()),Qt::DirectConnection);
    connect(video_thread, SIGNAL(finished()), videoWorker, SLOT(deleteLater()));
    connect(video_thread, SIGNAL(finished()), video_thread, SLOT(deleteLater()));
    //connect(this, SIGNAL(quitVideoThread()), video_thread, SLOT(quit()));
    video_thread->start();




    audio_thread = new QThread;
    audioWorker = new worker(audio_packq,audio_frame_q,audio_decCtx);
    audioWorker->moveToThread(audio_thread);

    connect(audio_thread, SIGNAL(started()), audioWorker, SLOT(work_thread()));
    connect(this, SIGNAL(quitAudioThread()), audioWorker, SLOT(quit_thread()),Qt::DirectConnection);
    connect(audio_thread, SIGNAL(finished()), audioWorker, SLOT(deleteLater()));
    connect(audio_thread, SIGNAL(finished()), audio_thread, SLOT(deleteLater()));
    //connect(this, SIGNAL(quitAudioThread()), audio_thread, SLOT(quit()));
    audio_thread->start();


}
void decode::stop(){
    emit quitVideoThread();
    video_thread->quit();

    emit quitAudioThread();
    audio_thread->quit();


}


