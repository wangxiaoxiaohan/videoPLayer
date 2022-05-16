#include "base/framequeue.h"
framequeue::framequeue()
{
    maxsize = 30;
    for(int i = 0 ; i < maxsize; i++){
        Frame frame;
        frame.af =av_frame_alloc();
        frame.used = false;
        frame_queue.push_back(frame);
    }
}
Frame* framequeue::getEmptyFrame(){
    Frame* frame;
    for(int i = 0 ; i < maxsize ; i++){
        mutex.lock();
        if(!frame_queue[i].used){
            printf("framequeue getEmptyFrame\n");
            frame = &frame_queue[i];
            mutex.unlock();
            return frame;
        }
        mutex.unlock();
    }
    return NULL;
}
void framequeue::put(){
    for(int i = 0 ; i < maxsize ; i++){
        mutex.lock();
        if(!frame_queue[i].used){
            printf("framequeue put\n");
            frame_queue[i].used = true;
            mutex.unlock();
            break;
        }
        mutex.unlock();
    }
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
