 #include "mediaplayer/player.h"
#include <QFile>
playThread::playThread(splayer *player,AVMediaType type):
    mMediaType(type),
    mPlayer(player)
{


}
void playThread::play_loop(){
    while(1){
    if(mMediaType == AVMEDIA_TYPE_VIDEO){
        Frame* vframe;
        while(1){
            vframe = mPlayer->decoder->video_frame_q->getAFullFrame();
            if(vframe){
                //qDebug() << "get a full video frame!!!!!!!!!!!!!!!!!!!!";
                break;
            }
            QThread::usleep(20000);
        }
        mPlayer->glwidget->slotShowYuv(vframe->af->data[0],vframe->af->data[1],vframe->af->data[2],1920,1080);

    }else if(mMediaType == AVMEDIA_TYPE_AUDIO){
            qDebug() << "start audio play loop";
            Frame* aframe = NULL;
            aframe = mPlayer->decoder->audio_frame_q->getAFullFrame();

            int data_size = av_get_bytes_per_sample(mPlayer->decoder->audio_decCtx->sample_fmt);
            if (data_size < 0) {
                qDebug() << "get size error";
                break;
            }

            qDebug() << "@@@@@@@@@sample size" << data_size;
            qDebug() << "@@@@@@@@@decoder->audio_decCtx->channels  " << aframe->af->channels;
            qDebug() << "@@@@@@@@@aframe->af->nb_samples  " << aframe->af->nb_samples;
            //int size = aframe->af->nb_samples * data_size * aframe->af->channels;
            int size = aframe->af->nb_samples * data_size;
            qDebug() << "@@@@@@@@@total size" << size;


            if (size <= 0) continue;

            char buf[size];
            int sum = 0;
            //qDebug()  << "audio format is " << av_sample_fmt_is_planar((AVSampleFormat)aframe->af->format);
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
            qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!audio pts" <<aframe->af->pts;

            double sleeptime = 1000000 * aframe->af->nb_samples * mPlayer->audio_timebase.num / mPlayer->audio_timebase.den;
            qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!audio sleep us:" <<sleeptime;
            QThread::usleep(sleeptime);


            mPlayer->audioOut->playRawAudio(QByteArray(buf,size));
            mPlayer->audio_clock = aframe->af->pts;


            mPlayer->decoder->audio_frame_q->pop();
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
}
splayer::~splayer(){


}

void splayer::play(){
    decoder->start(fmt_ctx);
    audioOut = new audioOutput();
    //to do ,set audio format
    play_loop();
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
        qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!video sleep us:" <<duration;

        while(1){
            double deviation = video_clock / video_timebase.den - audio_clock / audio_timebase.den;
            if(abs(deviation) < duration) break;
            if(deviation< 0){
                decoder->video_frame_q->pop();
                vframe = decoder->video_frame_q->getAFullFrame();
            }else{
                glwidget->slotShowYuv(vframe->af->data[0],vframe->af->data[1],vframe->af->data[2],1920,1080);
                QThread::usleep(duration);

            }
            video_clock = vframe->af->pts;

        }

        QThread::usleep(duration);
        glwidget->slotShowYuv(vframe->af->data[0],vframe->af->data[1],vframe->af->data[2],1920,1080);
        video_clock = vframe->af->pts;
        decoder->video_frame_q->pop();


    }
}
void splayer::pause(){


}
void splayer::stop(){


}
void splayer::seek(){

}
void splayer::setPlayRate(){


}
void splayer::prepare(QString file){
    QThread* thread = new QThread;

    char path[64];
    sprintf(path,"/home/wanghan/test/2000s.mp4");
    int ret  = demuxer->openFile(QString(path));
    audio_timebase = demuxer->fmtCtx->streams[demuxer->audio_steam_index]->time_base;
    video_timebase = demuxer->fmtCtx->streams[demuxer->video_steam_index]->time_base;

    qDebug() << "aduio time_base den:" << audio_timebase.den << "audio time_base num:" << audio_timebase.num ;
    qDebug() << "video time_base den:" << video_timebase.den << "video time_base num:" << video_timebase.num ;

    demuxer->moveToThread(thread);
    //connect(demuxer, SIGNAL(error(QString)), this, SLOT(errorString(QString)));

    connect(thread, SIGNAL(started()), demuxer, SLOT(read_thread()));
   // connect(demuxer, SIGNAL(finished()), thread, SLOT(quit()));

   // connect(demuxer, SIGNAL(finished()), demuxer, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();

}
