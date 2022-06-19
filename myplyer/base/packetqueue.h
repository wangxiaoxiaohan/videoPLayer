#ifndef PACKETQUEUE_H
#define PACKETQUEUE_H
extern "C"{
#include <libavformat/avformat.h>
}
#include <QQueue>
#include <QMutex>
#include <QLinkedList>
#include <QWaitCondition>
/*
typedef struct Packet_queue{
    QLinkedList<AVPacket> list;
    int size;
}Packet_queue;
*/

typedef struct Packet_Node{
    AVPacket packt;
    struct Packet_Node* next;
}Packet_Node;

typedef struct Packet_queue{
    Packet_Node *first,*last;
    int size;
}Packet_queue;
class packetqueue
{
public:
    packetqueue();
//    AVPacket get();
    AVPacket get(AVPacket *pkt);
    void put(AVPacket *pkt);
    int size();
    void clear();
private:
    int list_size;
    Packet_queue packet_queue;
    QWaitCondition condition;
    QMutex mutex;
};
#endif// PACKETQUEUE_H

