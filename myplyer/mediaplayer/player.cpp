 #include "mediaplayer/player.h"
#include <QFile>
#include <QCoreApplication>
playThread::playThread(splayer *player,AVMediaType type):
    mMediaType(type),
    mPlayer(player)
{


}
void playThread::play_loop(){
    while(mPlayer->mMediaStatus != media_stopped){
        if(mMediaType == AVMEDIA_TYPE_AUDIO){
            Frame* aframe = NULL;
            aframe = mPlayer->decoder->audio_frame_q->getAFullFrame();

            int data_size = av_get_bytes_per_sample(mPlayer->decoder->audio_decCtx->sample_fmt);
            if (data_size < 0) {
                qDebug() << "get size error";
                break;
            }
            int size = aframe->af->nb_samples * data_size;
            qDebug() << "@@@@@@@@@total size" << size << "  "<<aframe->af->nb_samples << "   "<<data_size;

            if (size <= 0){
                continue;
            }
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
             //qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!audio pts" <<aframe->af->pts;
            double sleeptime = 1000000 * aframe->af->nb_samples * mPlayer->audio_timebase.num / mPlayer->audio_timebase.den;
            //qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!audio sleep us:" <<sleeptime;
            QThread::usleep(sleeptime);

            mPlayer->audioOut->playRawAudio(QByteArray(buf,size));
            mPlayer->audio_clock = aframe->af->pts;
            av_frame_unref(aframe->af);
            mPlayer->decoder->audio_frame_q->pop();

            double Currentsecond =  mPlayer->audio_clock * mPlayer->audio_timebase.num / mPlayer->audio_timebase.den;
            double total = mPlayer->fmt_ctx->streams[mPlayer->demuxer->audio_steam_index]->duration *
                    mPlayer->audio_timebase.num / mPlayer->audio_timebase.den;
            emit mPlayer->updateProgress((int)Currentsecond,(int)total);

            while(mPlayer->mMediaStatus == media_paused){
                QThread::usleep(3000);
                //qDebug() << "vvvvvvvvv!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! audio not playing!!!!!! "<< mPlayer->mMediaStatus;
                QCoreApplication::processEvents(QEventLoop::AllEvents,100);
            }
           }
        }
        mPlayer->decoder->audio_frame_q->clear();
    qDebug() << "vvvvvvvvv!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! audio play loop exit";
}
splayer::splayer(glyuvwidget2* glw){
    demuxer = new demux();
    decoder = new decode();
    decoder->audio_packq =  demuxer->audio_packq;
    decoder->video_packq =  demuxer->video_packq;
    decoder->subtitle_packq =  demuxer->subtitle_packq;
    glwidget = glw;
    audio_clock = 0;
    video_clock = 0;
    mMediaStatus  = media_idle;
}
splayer::~splayer(){


}

void splayer::play(){
    // make flag to real play
    mMediaStatus = media_playing;
}
void splayer::reset(){
    mMediaStatus = media_idle;
}
void splayer::play_loop(){
    audio_th = new QThread;
    playThread *aP =  new playThread(this,AVMEDIA_TYPE_AUDIO);
    aP->moveToThread(audio_th);
    connect(audio_th, SIGNAL(started()), aP, SLOT(play_loop()));
    connect(audio_th, SIGNAL(finished()), audio_th, SLOT(deleteLater()));
    connect(audio_th, SIGNAL(finished()), aP, SLOT(deleteLater()));
    audio_th->start();
    qDebug() << "vvvvvvvvv!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 00000000" << video_clock;
    while(mMediaStatus != media_stopped){

        Frame* vframe;
        vframe = decoder->video_frame_q->getAFullFrame();
        if(video_clock == 0 ) video_clock = vframe->af->pts;

        double duration = 1000000 * (vframe->af->pts - video_clock)  * video_timebase.num / video_timebase.den;
        //qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!video sleep us:" <<duration;
        //qDebug() << "vvvvvvvvv!!!!!!!!!!!!!!!! vframe->af->pts  " << vframe->af->pts  << "video_clock  " << video_clock <<"video_timebase.num  "
        //          << video_timebase.num  << "  video_timebase.den" << video_timebase.den;
        //qDebug() << "vvvvvvvvv!!!!!!!!!!!!!!!! audio_clock  " << audio_clock <<"audio_timebase.num  "
        //          << audio_timebase.num  << "  audio_timebase.den" << audio_timebase.den;
        while(1){
            double deviation =  1000000 * (video_clock / video_timebase.den - audio_clock / audio_timebase.den);
            qDebug() << "vvvvvvvvv!!!!!!!!!!!!!!!! deviation " << deviation << "duration" << duration;
            if(abs(deviation) < duration || duration == 0) break;
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


        while(mMediaStatus == media_paused){
            QThread::usleep(3000);
            QCoreApplication::processEvents(QEventLoop::AllEvents,100);
        }
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
    }
    decoder->video_frame_q->clear();
    qDebug() << "vvvvvvvvv!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! video play loop exit";
    // fix me !!! only video can not ensure media state!!!
}
void splayer::pause(){
    mMediaStatus = media_paused;
}
void splayer::stop(){
    if(mMediaStatus == media_stopped)
        return;
    mMediaStatus = media_stopped;
    audio_th->quit();

    demuxer->stop();
    qDebug() << "demux quit done";

    decoder->stop();
    qDebug() << "decoder quit  done" ;
}

void splayer::seek(){

}
void splayer::setPlayRate(){


}
enum mediaStatus splayer::mediaStatus(){
    return mMediaStatus;
}
void splayer::prepare(QString file){
    if(mMediaStatus == media_playing)
        return;
    mMediaStatus = media_paused;
// start demuxer
    int ret  = demuxer->openFile(file);
    qDebug() << "demux openfile done ";
    audio_timebase = demuxer->fmtCtx->streams[demuxer->audio_steam_index]->time_base;
    video_timebase = demuxer->fmtCtx->streams[demuxer->video_steam_index]->time_base;
    demuxer->start(file);
    qDebug() << "demux start done ";

// start decoder
    fmt_ctx = demuxer->fmtCtx;
    audio_clock = 0;
    video_clock = 0;
    decoder->start(fmt_ctx);
    qDebug() << "decode start done ";


// start play loop
    audioOut = new audioOutput();
    // to do,set audio format
    play_loop();
}
