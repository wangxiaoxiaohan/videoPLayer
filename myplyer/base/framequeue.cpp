#include "base/framequeue.h"
#include <QDebug>
#include "libavutil/mem.h"
/*
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
*/
framequeue::framequeue()
{
    maxsize = 30;
    shownsize = 0;
    fullsize = 0;
    write_index = 0;
    read_index = 0;
    for(int i = 0 ; i < maxsize; i++){
        Frame *frame;
        frame = (Frame*)av_malloc(sizeof (Frame));
        frame->af =av_frame_alloc();
        frame->used = false;
        frame_queue.push_back(frame);
    }
}
Frame* framequeue::getEmptyFrame()
{
    mutex.lock();
    while(frame_queue[write_index]->used)
        condition.wait(&mutex);
    mutex.unlock();
    return frame_queue[write_index];

}
void framequeue::put(){
    mutex.lock();
    frame_queue[write_index]->used = true;
    if(write_index + 1 == maxsize)
        write_index = 0;
    else
        write_index++;

    condition.wakeAll();
    mutex.unlock();
}
Frame* framequeue::getAFullFrame(){
    mutex.lock();
    while(!frame_queue[read_index]->used)
        condition.wait(&mutex);
    mutex.unlock();
    return frame_queue[read_index];
}
void framequeue::pop(){
    mutex.lock();
    frame_queue[read_index]->used = false;
    if(read_index + 1 == maxsize)
        read_index = 0;
    else
        read_index++;

    condition.wakeAll();
    mutex.unlock();
}




