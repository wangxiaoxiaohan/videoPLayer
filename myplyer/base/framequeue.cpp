#include "base/framequeue.h"
#include <QDebug>
framequeue::framequeue()
{
    maxsize = 30;
    shownsize = 0;
    fullsize = 0;
    write_index = 0;
    read_index = 0;
    for(int i = 0 ; i < maxsize; i++){
        Frame frame;
        frame.af =av_frame_alloc();
        frame.used = false;
        frame_queue.push_back(frame);
    }
}
/*
Frame* framequeue::getEmptyFrame(int *index){
    Frame* frame;
    for(int i = 0 ; i < maxsize ; i++){
        mutex.lock();
        if(!frame_queue[i].used){
            qDebug()<< "framequeue getEmptyFrame";
            frame = &frame_queue[i];
            mutex.unlock();
            *index = i;
            return frame;
        }
        mutex.unlock();
    }
    *index = -1;
    return NULL;
}
void framequeue::put(int index){
    //for(int i = 0 ; i < maxsize ; i++){
        mutex.lock();
        if(!frame_queue[index].used){
            qDebug() <<"framequeue put";
            frame_queue[index].used = true;
        }else{
            qDebug() << "frame is already full!!";
        }

        mutex.unlock();
    //}
}
Frame* framequeue::getAFullFrame(){
    Frame *frame;
    for(int i = 0 ; i < maxsize ; i++){
        mutex.lock();
        if(frame_queue[i].used){
            printf("getAFullFrame 222222222\n");
            frame = &frame_queue[i];
            frame_queue[i].used = false;
            mutex.unlock();   
            return frame;
        }
        mutex.unlock();
    }
    return NULL;
}
*/
/**************************************************/

Frame* framequeue::getEmptyFrame()
{
    mutex.lock();
    if(frame_queue[write_index].used)
        condition.wait(&mutex);
    mutex.unlock();
    return &frame_queue[write_index];

}
void framequeue::put(){
    mutex.lock();
    frame_queue[write_index].used = true;
    if(write_index + 1 == maxsize)
        write_index = 0;
    else
        write_index++;

    condition.wakeAll();
    mutex.unlock();
}
Frame* framequeue::getAFullFrame(){
    mutex.lock();
    qDebug() << "get frame index" <<read_index;
    if(!frame_queue[read_index].used)
        condition.wait(&mutex);
    mutex.unlock();
    return &frame_queue[read_index];
}
void framequeue::pop(){
    mutex.lock();
    frame_queue[read_index].used = false;
    if(read_index + 1 == maxsize)
        read_index = 0;
    else
        read_index++;

    condition.wakeAll();
    mutex.unlock();
}





