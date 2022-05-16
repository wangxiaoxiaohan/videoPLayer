#include "base/packetqueue.h"

packetqueue::packetqueue()
    :list_size(0)
{

}
AVPacket packetqueue::get(){
    printf("packetqueue get\n");
    AVPacket pkt ;
    mutex.lock();
    if(!packet_queue.list.empty()){
        pkt  = packet_queue.list.front();
        packet_queue.list.pop_front();
        mutex.unlock();
        return pkt;
    }

    mutex.unlock();
    return pkt;

}
void packetqueue::put(AVPacket *pkt){
     printf("packetqueue put\n");
    mutex.lock();

    packet_queue.list.push_back(*pkt);
   // list_size +=1;
    mutex.unlock();
}
int packetqueue::size(){
    return packet_queue.list.size();
}
