#include "base/packetqueue.h"

packetqueue::packetqueue()
    :list_size(0)
{
    packet_queue.size = 0;
    packet_queue.first = NULL;
    packet_queue.last = NULL;
}
//packetqueue still exist memory lack

AVPacket packetqueue::get(AVPacket *pkt){
   // printf("packetqueue get\n");
    mutex.lock();
    while(packet_queue.size == 0)
        condition.wait(&mutex);

    Packet_Node *node = packet_queue.first;
    *pkt = node->packt;
    packet_queue.first = node->next;
    av_free(node);
    packet_queue.size -= 1;

    mutex.unlock();
}

void packetqueue::put(AVPacket *pkt){
   //  printf("packetqueue put\n");
    mutex.lock();
    Packet_Node *node;
    node = (Packet_Node *)av_malloc(sizeof(Packet_Node));
    node->next = NULL;
    node->packt = *pkt;

    if(NULL == packet_queue.first){
        packet_queue.first = node;
        packet_queue.last = node;
    }else{
        packet_queue.last->next = node;
        packet_queue.last = node;

    }
    packet_queue.size += 1;
    condition.wakeAll();
    mutex.unlock();
}

int packetqueue::size(){
    return packet_queue.size;
}
void packetqueue::clear(){
    mutex.lock();
    while(packet_queue.first != packet_queue.last){
        Packet_Node *node = packet_queue.first;
        packet_queue.first = node ->next;
        av_free(node);
    }
    //av_packet_unref(&packet_queue.first->packt);
    av_free(packet_queue.first);
    packet_queue.size = 0;
    packet_queue.first = NULL;
    packet_queue.last = NULL;
    mutex.unlock();
}
