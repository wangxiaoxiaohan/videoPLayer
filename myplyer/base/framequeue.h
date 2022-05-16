#ifndef QUEUE_H
#define QUEUE_H
#include <QQueue>
#include <QMutex>
#include <demuxer/demux.h>
typedef struct Frame{
    int pts;
    AVFrame *af;
    bool used;
}Frame;
class framequeue
{
public:
    framequeue();
    Frame *getEmptyFrame();
    Frame *getAFullFrame();
    void put();
private:
    QMutex mutex;
    QQueue<Frame> frame_queue;
    int maxsize;

};

#endif // QUEUE_H
