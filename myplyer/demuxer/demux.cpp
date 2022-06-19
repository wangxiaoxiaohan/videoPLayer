#include "demuxer/demux.h"
#include <QtDebug>
#include <QCoreApplication>
demuxWorker::demuxWorker(demux *demuxer)
    :mDemuxer(demuxer),
    quit_flag(false)
{


}
demuxWorker::~demuxWorker(){
    qDebug() << "demuxWorker destruct!!";
}
void demuxWorker::quit_threag_flag(){
    qDebug() << "demux receive quit signal";
    quit_flag = true;
}
void demuxWorker::work_thread(){
    //mDemuxer->read_thread();
    int ret;
    AVPacket pkt;
    qDebug() << "in demux work_thread";
    while(!quit_flag){
        qDebug() << "in demux work_thread reading";
        if(mDemuxer->fmtCtx){
            while((mDemuxer->audio_packq->size() > 1000) || mDemuxer->video_packq->size() > 500 ){
                QThread::usleep(3000);
                QCoreApplication::processEvents(QEventLoop::AllEvents,100);
                qDebug() << "packet full!!!";
            }
            int ret = av_read_frame(mDemuxer->fmtCtx,&pkt);
            if(ret < 0) continue;
            if(pkt.stream_index == mDemuxer->audio_steam_index){
                qDebug() << "read an audio packet size:";
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
    qDebug() << "demux real quit  threading!!!!!!!!!!!";
    avformat_close_input(&mDemuxer->fmtCtx);
    mDemuxer->video_packq->clear();
    mDemuxer->audio_packq->clear();
}
demux::demux()
{
    //fmtCtx = avformat_alloc_context();
    video_packq = new packetqueue();
    audio_packq = new packetqueue();
    subtitle_packq = new packetqueue();

}
int demux::openFile(QString path){
    qDebug() << path;
    QByteArray ba = path.toLatin1();

    // to do, free last fmtctx or leak memory;
    fmtCtx = avformat_alloc_context();

    qDebug() << "open input done asd";
    if (avformat_open_input(&fmtCtx, ba.data(), NULL, NULL) < 0) {
        qDebug() << "Could not open source file " <<  ba.data();
        exit(1);
    }
    qDebug() << "open input done";
    if (avformat_find_stream_info(fmtCtx, NULL) < 0) {
        qDebug() << "Could not find stream information";
        exit(1);
    }
    qDebug() << "avformat_find_stream_info done";
    audio_steam_index = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);

    video_steam_index = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);


}

int demux::findStreamInfo(){



}
int demux::start(QString file){
    video_packq->clear();
    audio_packq->clear();

    worker = new demuxWorker(this);
    demuxThread = new QThread;
    worker->moveToThread(demuxThread);
    connect(demuxThread, SIGNAL(started()), worker, SLOT(work_thread()));
    connect(this, SIGNAL(quitDemuxThread()), worker, SLOT(quit_threag_flag()),Qt::DirectConnection);
    connect(demuxThread, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(demuxThread, SIGNAL(finished()), demuxThread, SLOT(deleteLater()));
    //emit quitDemuxThread(false);
    demuxThread->start();
}
int demux::stop(){

    qDebug() << "receive demux quit ";
    emit quitDemuxThread();
    video_packq->clear();
    audio_packq->clear();
    demuxThread->quit();

}
int demux::read_thread(){

}
