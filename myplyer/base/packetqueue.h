#ifndef PACKETQUEUE_H
#define PACKETQUEUE_H
extern "C"{
#include <libavformat/avformat.h>
}
#include <QQueue>
#include <QMutex>
#include <QLinkedList>

typedef struct Packet_queue{
    QLinkedList<AVPacket> list;
    int size;
}Packet_queue;

class packetqueue
{
public:
    packetqueue();
    AVPacket get();
    void put(AVPacket *pkt);
    int size();
private:
    int list_size;
    Packet_queue packet_queue;

    QMutex mutex;
};
#endif// PACKETQUEUE_H

