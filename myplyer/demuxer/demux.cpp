#include "demuxer/demux.h"
#include <QtDebug>
#include <QCoreApplication>
demuxWorker::demuxWorker(demux *demuxer)
    :mDemuxer(demuxer),
    quit_flag(false)
{


}
void demuxWorker::quit_thread(){
    quit_flag = true;

}
void demuxWorker::work_thread(){
    //mDemuxer->read_thread();
    int ret;
    AVPacket pkt;
    while(!quit_flag){

        if(mDemuxer->fmtCtx){
            while((mDemuxer->audio_packq->size() > 1000) || mDemuxer->video_packq->size() > 500 ) QThread::usleep(3000);
            int ret = av_read_frame(mDemuxer->fmtCtx,&pkt);
            if(ret < 0) continue;
            if(pkt.stream_index == mDemuxer->audio_steam_index){
               // qDebug() << "read an audio packet size:" << audio_packq->size();
                mDemuxer->audio_packq->put(&pkt);
                //qDebug() << "audio pkt pts" << pkt.pts;
            }else if(pkt.stream_index == mDemuxer->video_steam_index){
               // qDebug() << "PUT LIST SIZE" << video_packq->size();
                mDemuxer->video_packq->put(&pkt);
              // qDebug() << "read a video packet size:" << pkt.size;
            }else{
              // subtitle_packq->put(&pkt);
                av_packet_unref(&pkt);
            }
        }
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
    }
}
demux::demux()
{
    fmtCtx = avformat_alloc_context();
    video_packq = new packetqueue();
    audio_packq = new packetqueue();
    subtitle_packq = new packetqueue();
    worker = new demuxWorker(this);
    demuxThread = new QThread;
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
    audio_steam_index = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);

    video_steam_index = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);


}

int demux::findStreamInfo(){



}
int demux::srart(QString file){
    worker->moveToThread(demuxThread);
    connect(demuxThread, SIGNAL(started()), worker, SLOT(work_thread()));
    connect(this, SIGNAL(quitDemuxThread()), worker, SLOT(quit_thread()));
  //  connect(demuxThread, SIGNAL(finished()), worker, SLOT(deleteLater()));
  //  connect(demuxThread, SIGNAL(finished()), demuxThread, SLOT(deleteLater()));
    demuxThread->start();

}
int demux::stop(){
   //demuxThread->quit();
    qDebug() << "receive demux quit ";
    emit quitDemuxThread();
}
int demux::read_thread(){

}
