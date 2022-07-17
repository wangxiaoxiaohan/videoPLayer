#include "mediaplayer/player.h"
#include <QFile>
#include <QCoreApplication>

playThread::playThread(splayer *player,AVMediaType type):
    mMediaType(type),
    mPlayer(player)
{


}
void playThread::play_loop(){
        if(mMediaType == AVMEDIA_TYPE_AUDIO){
            while(mPlayer->mMediaStatus != media_stopped){
            Frame* aframe = NULL;
            aframe = mPlayer->decoder->audio_frame_q->getAFullFrame();
            if(mPlayer->audio_clock == 0 ) mPlayer->audio_clock = aframe->af->pts;
            int data_size = av_get_bytes_per_sample(mPlayer->decoder->audio_decCtx->sample_fmt);
            if (data_size < 0) {
                qDebug() << "get size error";
                break;
            }
            int size = aframe->af->nb_samples * data_size;
            qDebug() << "@@@@@@@@@total size" << size << "  "<<aframe->af->nb_samples << "   "<<data_size;

            // fix me !! if contine here ,this frame will not pop ,cause died loop;
            if (size <= 0){
                mPlayer->decoder->audio_frame_q->pop();
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
            qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!mPlayer->audio_clock" <<mPlayer->audio_clock;
            double sleeptime = 1000000 * aframe->af->nb_samples * mPlayer->audio_timebase.num / mPlayer->audio_timebase.den;
            qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!audio sleep us:" <<sleeptime;
            QThread::usleep(sleeptime / mPlayer->playSpeed);
            qDebug() << "audio loop 000";
            mPlayer->audioOut->playRawAudio(QByteArray(buf,size));
            mPlayer->audio_clock = aframe->af->pts;
            av_frame_unref(aframe->af);
            mPlayer->decoder->audio_frame_q->pop();
            qDebug() << "audio loop 111";
            double Currentsecond =  mPlayer->audio_clock * mPlayer->audio_timebase.num / mPlayer->audio_timebase.den;
            double total = mPlayer->fmt_ctx->streams[mPlayer->demuxer->audio_steam_index]->duration *
                    mPlayer->audio_timebase.num / mPlayer->audio_timebase.den;
            emit mPlayer->updateProgress((int)Currentsecond,(int)total);
            qDebug() << "audio loop 222";
            while(mPlayer->mMediaStatus == media_paused){
                QThread::usleep(3000);
                //qDebug() << "vvvvvvvvv!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! audio not playing!!!!!! "<< mPlayer->mMediaStatus;
                QCoreApplication::processEvents(QEventLoop::AllEvents,100);
            }
            while(mPlayer->mMediaStatus == media_seeking){
                mPlayer->audio_clock = 0;
                QThread::usleep(3000);
                qDebug() << "audio play loop seeking!!!";
                QCoreApplication::processEvents(QEventLoop::AllEvents,100);
            }

            }
            mPlayer->decoder->audio_frame_q->clear();
            qDebug() << "vvvvvvvvv!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! audio play loop exit";
        }else{
            while(mPlayer->mMediaStatus != media_stopped){

                Frame* vframe;
                vframe = mPlayer->decoder->video_frame_q->getAFullFrame();
                if(mPlayer->video_clock == 0 ) mPlayer->video_clock = vframe->af->pts;

                double duration = 1000000 * (vframe->af->pts - mPlayer->video_clock)  * mPlayer->video_timebase.num / mPlayer->video_timebase.den;
                qDebug() << "play loop 1111";
                while(1){
                    double deviation =  1000000 * (mPlayer->video_clock / mPlayer->video_timebase.den - mPlayer->audio_clock / mPlayer->audio_timebase.den);
                    qDebug() << "video_clock" << mPlayer->video_clock;
                    qDebug() << "vvvvvvvvv!!!!!!!!!!!!!!!! deviation " << deviation << "duration" << duration;
                    if(abs(deviation) < duration || duration == 0) break;
                    if(mPlayer->mMediaStatus == media_seeking) break;

                    if(deviation< 0){
                        av_frame_unref(vframe->af);
                        mPlayer->decoder->video_frame_q->pop();
                        vframe = mPlayer->decoder->video_frame_q->getAFullFrame();
                    }else{
                        memcpy(mPlayer->y_ptr_front,vframe->af->data[0], mPlayer->bufSize_y);
                        memcpy(mPlayer->u_ptr_front,vframe->af->data[1], mPlayer->bufSize_u);
                        memcpy(mPlayer->v_ptr_front,vframe->af->data[2], mPlayer->bufSize_v);

                        emit mPlayer->vSync();
                        QThread::usleep(duration);
                    }
                    mPlayer->video_clock = vframe->af->pts;
                    QCoreApplication::processEvents(QEventLoop::AllEvents,100);
                }
                qDebug() << "play loop 22222";
                QThread::usleep(duration / mPlayer->playSpeed);

                memcpy(mPlayer->y_ptr_front,vframe->af->data[0], mPlayer->bufSize_y);
                memcpy(mPlayer->u_ptr_front,vframe->af->data[1], mPlayer->bufSize_u);
                memcpy(mPlayer->v_ptr_front,vframe->af->data[2], mPlayer->bufSize_v);
                emit mPlayer->vSync();

                mPlayer->video_clock = vframe->af->pts;
                qDebug() << "play loop 3333";
                av_frame_unref(vframe->af);
                mPlayer->decoder->video_frame_q->pop();
                qDebug() << "play loop 4444";

                while(mPlayer->mMediaStatus == media_paused){
                    QThread::usleep(3000);
                    qDebug() << "play loop pausing!!!";
                    QCoreApplication::processEvents(QEventLoop::AllEvents,100);
                }
                while(mPlayer->mMediaStatus == media_seeking){
                    mPlayer->video_clock = 0;
                    QThread::usleep(3000);
                    qDebug() << "video play loop seeking!!!";
                    QCoreApplication::processEvents(QEventLoop::AllEvents,100);
                }
                QCoreApplication::processEvents(QEventLoop::AllEvents,100);
            }
            mPlayer->decoder->video_frame_q->clear();
            qDebug() << "vvvvvvvvv!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! video play loop exit";
        }
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
    playSpeed = 1.0;
    //fix me!!!
    bufSize_y = 1920 * 1080;
    bufSize_u = 1920 * 1080 / 4;
    bufSize_v = 1920 * 1080 / 4;
    y_ptr_front = new  unsigned char[bufSize_y];
    u_ptr_front = new  unsigned char[bufSize_u];
    v_ptr_front = new  unsigned char[bufSize_v];

    y_ptr_back = new  unsigned char[bufSize_y];
    u_ptr_back = new  unsigned char[bufSize_u];
    v_ptr_back = new  unsigned char[bufSize_v];

    connect(this,SIGNAL(vSync()),this,SLOT(vSyncSlot()));


    qDebug() << "main thread id" << QThread::currentThreadId();
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

    video_th = new QThread;
    playThread *vP =  new playThread(this,AVMEDIA_TYPE_VIDEO);
    vP->moveToThread(video_th);
    connect(video_th, SIGNAL(started()), vP, SLOT(play_loop()));
    connect(video_th, SIGNAL(finished()), video_th, SLOT(deleteLater()));
    connect(video_th, SIGNAL(finished()), vP, SLOT(deleteLater()));
    video_th->start();
}
void splayer::pause(){
    mMediaStatus = media_paused;
}
void splayer::stop(){
    stopState();
    QThread::usleep(30000);
    stopDecoderDemuxer();
}
void splayer::stopState(){
    if(mMediaStatus == media_stopped)
        return;
    mMediaStatus = media_stopped;
    audio_th->quit();

}
void splayer::stopDecoderDemuxer(){
    demuxer->stop();
    qDebug() << "demux quit done";

    decoder->stop();
    qDebug() << "decoder quit  done" ;
}

void splayer:: seek(double precent){

    mMediaStatus = media_seeking;

    //pause decode
    decoder->pauseDecode();
    QThread::usleep(30000);
    //seek
    double total =fmt_ctx->duration;
          //  *audio_timebase.num / audio_timebase.den;
    double timestamp = total * precent / 1000;
    qDebug() << "demuxer->seek  total " << total << "percent" << precent << "timestamp" <<timestamp;
    demuxer->seek(timestamp);

    QThread::usleep(30000);

    qDebug() << "seek flush decode && clear frame queue && continue decoder";

    decoder->continueDecode();

    QThread::usleep(30000);
    qDebug() << " seek play_loop";
    //restart playloop
    play();
    qDebug() << " seek play";
    //play_loop();
}
void splayer::setPlayRate(double speed){
    playSpeed = speed;
}
double splayer::playRate(){
    return playSpeed;
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
void splayer::vSyncSlot(){
    qDebug() << "vSync thread id" << QThread::currentThreadId();

    /*
     * when ew receice signal to here , front buffer has full
     * swap front and back buffer at once
     * then render back buffer
    */
    uchar* y_ptr = y_ptr_front;
    uchar* u_ptr = u_ptr_front;
    uchar* v_ptr = v_ptr_front;

    y_ptr_front = y_ptr_back;
    u_ptr_front = u_ptr_back;
    v_ptr_front = v_ptr_back;

    y_ptr_back = y_ptr;
    u_ptr_back = u_ptr;
    v_ptr_back = v_ptr;

    glwidget->slotShowYuv(y_ptr_back,u_ptr_back,v_ptr_back,1920,1080);


}
