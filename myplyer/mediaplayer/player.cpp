 #include "mediaplayer/player.h"
#include <QFile>
#include <QCoreApplication>
playThread::playThread(splayer *player,AVMediaType type):
    mMediaType(type),
    mPlayer(player)
{


}
void playThread::play_loop(){
    while(1){
        if(mMediaType == AVMEDIA_TYPE_AUDIO){
            //qDebug() << "start audio play loop";

            Frame* aframe = NULL;
            aframe = mPlayer->decoder->audio_frame_q->getAFullFrame();

            int data_size = av_get_bytes_per_sample(mPlayer->decoder->audio_decCtx->sample_fmt);
            if (data_size < 0) {
                qDebug() << "get size error";
                break;
            }

            //qDebug() << "@@@@@@@@@sample size" << data_size;
            //qDebug() << "@@@@@@@@@decoder->audio_decCtx->channels  " << aframe->af->channels;
           // qDebug() << "@@@@@@@@@aframe->af->nb_samples  " << aframe->af->nb_samples;
            //int size = aframe->af->nb_samples * data_size * aframe->af->channels;
            int size = aframe->af->nb_samples * data_size;
           // qDebug() << "@@@@@@@@@total size" << size;


            if (size <= 0) continue;

            char buf[size];
            int sum = 0;
            // just play only one channel temporarily
            int isplanar =  false;//av_sample_fmt_is_planar((AVSampleFormat)aframe->af->format);

            if(isplanar){
                for(int i = 0 ; i < aframe->af->nb_samples ; i++){
                                qDebug() << "sum" << sum;
                    for (int ch = 0; ch < mPlayer->decoder->audio_decCtx->channels; ch++)
                     {
                       if(!aframe->af->data[ch]){
                            qDebug() << "pointer is NULL !!! SUM :" << sum;
                            continue;
                         }
                         memcpy(buf+sum, aframe->af->data[ch] + data_size*i,data_size);
                         sum+=data_size;
                     }

                }
            }else{
                // some avframe contains null data
                if(!aframe->af->data[0]){
                    qDebug() << "DATA 0 IS NUll";
                    continue;
                }
                memcpy(buf,aframe->af->data[0],size);

            }
            // qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!audio pts" <<aframe->af->pts;
            double sleeptime = 1000000 * aframe->af->nb_samples * mPlayer->audio_timebase.num / mPlayer->audio_timebase.den;
            //qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!audio sleep us:" <<sleeptime;
            QThread::usleep(sleeptime);

            mPlayer->audioOut->playRawAudio(QByteArray(buf,size));
            mPlayer->audio_clock = aframe->af->pts;

            av_frame_unref(aframe->af);
            mPlayer->decoder->audio_frame_q->pop();


            while(mPlayer->mMediaStatus != media_playing){
                QThread::usleep(3000);
                QCoreApplication::processEvents(QEventLoop::AllEvents,100);
            }
           }
        }
}
splayer::splayer(glyuvwidget2* glw){
    demuxer = new demux();
    decoder = new decode();
    decoder->audio_packq =  demuxer->audio_packq;
    decoder->video_packq =  demuxer->video_packq;
    decoder->subtitle_packq =  demuxer->subtitle_packq;
    fmt_ctx = demuxer->fmtCtx;
    glwidget = glw;
    audio_clock = 0;
    video_clock = 0;
    mMediaStatus  = media_stopped;
}
splayer::~splayer(){


}

void splayer::play(){
    // make flag to real play
    mMediaStatus = media_playing;
}
void splayer::play_loop(){
    QThread* audio_th = new QThread;
    playThread *aP =  new playThread(this,AVMEDIA_TYPE_AUDIO);
    aP->moveToThread(audio_th);
    connect(audio_th, SIGNAL(started()), aP, SLOT(play_loop()));
    connect(audio_th, SIGNAL(finished()), aP, SLOT(deleteLater()));
    audio_th->start();

    while(1){

        Frame* vframe;
        vframe = decoder->video_frame_q->getAFullFrame();
        if(video_clock == 0 ) video_clock = vframe->af->pts;


        double duration = 1000000 * (vframe->af->pts - video_clock)  * video_timebase.num / video_timebase.den;
        //qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!video sleep us:" <<duration;
        while(1){
            double deviation = video_clock / video_timebase.den - audio_clock / audio_timebase.den;
            if(abs(deviation) < duration) break;
            if(deviation< 0){
                av_frame_unref(vframe->af);
                decoder->video_frame_q->pop();
                vframe = decoder->video_frame_q->getAFullFrame();
            }else{
                glwidget->slotShowYuv(vframe->af->data[0],vframe->af->data[1],vframe->af->data[2],1920,1080);
                QThread::usleep(duration);
            }
            video_clock = vframe->af->pts;
            QCoreApplication::processEvents(QEventLoop::AllEvents,100);
        }

        QThread::usleep(duration);
        glwidget->slotShowYuv(vframe->af->data[0],vframe->af->data[1],vframe->af->data[2],1920,1080);
        video_clock = vframe->af->pts;

        av_frame_unref(vframe->af);
        decoder->video_frame_q->pop();


        while(mMediaStatus != media_playing){
            QThread::usleep(3000);
            QCoreApplication::processEvents(QEventLoop::AllEvents,100);
        }
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
    }

}
void splayer::pause(){
    mMediaStatus = media_paused;
}
void splayer::stop(){
   // mMediaStatus = media_stopped;
    decoder->stop();
    qDebug() << "decoder quit  done" ;
    demuxer->stop();
    qDebug() << "demux quit done" ;

}
void splayer::seek(){

}
void splayer::setPlayRate(){


}
enum mediaStatus splayer::mediaStatus(){
    return mMediaStatus;
}
void splayer::prepare(QString file){
// start demuxer
    int ret  = demuxer->openFile(file);
    audio_timebase = demuxer->fmtCtx->streams[demuxer->audio_steam_index]->time_base;
    video_timebase = demuxer->fmtCtx->streams[demuxer->video_steam_index]->time_base;
    demuxer->srart(file);
// start decoder
    decoder->start(fmt_ctx);
// start play loop
    audioOut = new audioOutput();
    play_loop();
}
