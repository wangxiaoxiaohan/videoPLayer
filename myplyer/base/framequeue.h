#ifndef QUEUE_H
#define QUEUE_H
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
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
    /*
    Frame *getEmptyFrame(int *index);
    Frame *getAFullFrame();
    void put(int index);
    */
    Frame* getEmptyFrame();
    void put();
    Frame* getAFullFrame();
    void pop();


private:
    QMutex mutex;
    QQueue<Frame *> frame_queue;
    QWaitCondition condition;

    int maxsize;
    int fullsize;
    int shownsize;
    int write_index;
    int read_index;

};

#endif // QUEUE_H
