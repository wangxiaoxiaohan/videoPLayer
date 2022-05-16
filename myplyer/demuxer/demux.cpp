#include "demuxer/demux.h"
#include <QtDebug>
demux::demux()
{
    fmtCtx = avformat_alloc_context();
    video_packq = new packetqueue();
    audio_packq = new packetqueue();
    subtitle_packq = new packetqueue();
}
int demux::openFile(QString path){
    qDebug() << path;
    QByteArray ba = path.toLatin1();
    if (avformat_open_input(&fmtCtx, ba.data(), NULL, NULL) < 0) {
        qDebug() << "Could not open source file " <<  ba.data();
        exit(1);
    }
    if (avformat_find_stream_info(fmtCtx, NULL) < 0) {
        qDebug() << "Could not find stream information";
        exit(1);
    }

}

int demux::findStreamInfo(){



}
int demux::srart(QString uri){


}
int demux::read_thread(){
    int ret;
    qDebug() << "read_thread 1111";

    AVPacket pkt;
   // av_init_packet(&pkt);
   // pkt.data = NULL;
   // pkt.size = 0;

    qDebug() << "read_thread 222";
    audio_steam_index = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);

    video_steam_index = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);

    subtitle_steam_index = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_SUBTITLE, -1, -1, NULL, 0);
    qDebug() << "read_thread 33333";
    while(1){
        if(fmtCtx){
            if(audio_packq->size() > 500) QThread::usleep(1000);
           //  if(video_packq->size() > 500) QThread::usleep(10000);
            int ret = av_read_frame(fmtCtx,&pkt);
            if(ret < 0) continue;
            if(pkt.stream_index == audio_steam_index){
               // qDebug() << "read an audio packet size:" << audio_packq->size();

                audio_packq->put(&pkt);
            }else if(pkt.stream_index == video_steam_index){
               // qDebug() << "PUT LIST SIZE" << video_packq->size();

               // video_packq->put(&pkt);
              // qDebug() << "read a video packet size:" << pkt.size;
            }else{
              // subtitle_packq->put(&pkt);
            }
        }

    }

}
