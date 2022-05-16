#include "mediaplayer/player.h"
#include <QFile>
splayer::splayer(glyuvwidget2* glw){
    demuxer = new demux();
    decoder = new decode();
    decoder->audio_packq =  demuxer->audio_packq;
    decoder->video_packq =  demuxer->video_packq;
    decoder->subtitle_packq =  demuxer->subtitle_packq;
    fmt_ctx = demuxer->fmtCtx;
    glwidget = glw;

}
splayer::~splayer(){


}
/*
QByteArray generatePCM()
{
    //幅度，因为sampleSize = 16bit
    qint16 amplitude = INT16_MAX;
    //单声道
    int channels = 1;
    //采样率
    int samplerate = 8000;
    //持续时间ms
    int duration = 20;
    //总样本数
    int n_samples = int(channels * samplerate * (duration / 1000.0));
    //声音频率
    int frequency = 100;

    bool reverse = false;
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::LittleEndian);
    for (int i = 0; i < 1000; i++) {
        for (int j = 0; j < n_samples; j++) {
            qreal radians = qreal(2.0 * M_PI * j  * frequency / qreal(samplerate));
            qint16 sample = qint16(qSin(radians) * amplitude);
            out << sample;
        }

        if (!reverse) {
            if (frequency < 2000) {
                frequency += 100;
            } else reverse = true;
        } else {
            if (frequency > 100) {
                frequency -= 100;
            } else reverse = false;
        }
    }

    QFile file("raw");
    file.open(QIODevice::WriteOnly);
    file.write(data);
    file.close();

    return data;
}
*/
void splayer::play(){
    decoder->start(fmt_ctx);
    audioOut = new audioOutput();
   // audioOut->setAudioFormat(QString("audio/pcm"),decoder->audio_decCtx->sample_rate,)
    play_loop();
}
void splayer::play_loop(){
    QFile file("/home/wanghan/test/qpcm.pcm");
    file.open(QIODevice::ReadWrite);
    while(1){
/*
        Frame* vframe;
        while(1){
            vframe = decoder->video_frame_q->getAFullFrame();
            if(vframe){
                qDebug() << "get a full video frame!!!!!!!!!!!!!!!!!!!!";
                break;
            }
            QThread::usleep(20000);
        }
        glwidget->slotShowYuv(vframe->af->data[0],vframe->af->data[1],vframe->af->data[2],1920,1080);
*/
/**/
        Frame* aframe = NULL;

        while(1){
            aframe = decoder->audio_frame_q->getAFullFrame();
            if(aframe != NULL){
                qDebug() << "get a full audio frame!!!!!!!!!!!!!!!!!!!!";
                break;
           }
            QThread::usleep(20000);
        }

        int data_size = av_get_bytes_per_sample(decoder->audio_decCtx->sample_fmt);
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
        qDebug()  << "audio format is " << av_sample_fmt_is_planar((AVSampleFormat)aframe->af->format);
        int isplanar =  false;//av_sample_fmt_is_planar((AVSampleFormat)aframe->af->format);

        if(isplanar){
            for(int i = 0 ; i < aframe->af->nb_samples ; i++){
                            qDebug() << "sum" << sum;
                for (int ch = 0; ch < decoder->audio_decCtx->channels; ch++)
                 {
                   if(!aframe->af->data[ch]){
                        qDebug() << "pointer is NULL !!! SUM :" << sum;
                        continue;
                     }
                    qDebug() << "strlen : " << strlen((char *)aframe->af->data[ch]);

                     memcpy(buf+sum, aframe->af->data[ch] + data_size*i,data_size);
                     qDebug() << "play loop bbb222";
                     sum+=data_size;
                 }

            }
        }else{
            // some avframe contains null data
            if(!aframe->af->data[0]){
                qDebug() << "DATA 0 IS NUll";
                continue;
            }

            //qDebug() << "data 0 size" << strlen((char *)aframe->af->data[0]);
            memcpy(buf,aframe->af->data[0],size);

        }
        file.write(buf,size);
     //   audioOut->playRawAudio(QByteArray(buf));
     //   QThread::usleep(20000);

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
    sprintf(path,"/home/wanghan/test/sample.mp4");
    int ret  = demuxer->openFile(QString(path));
    qDebug() << "openfile ret " << ret;
    demuxer->moveToThread(thread);
    //connect(demuxer, SIGNAL(error(QString)), this, SLOT(errorString(QString)));

    connect(thread, SIGNAL(started()), demuxer, SLOT(read_thread()));
   // connect(demuxer, SIGNAL(finished()), thread, SLOT(quit()));

   // connect(demuxer, SIGNAL(finished()), demuxer, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();

}
