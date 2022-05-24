#include "audiodevice.h"

audioDevice::audioDevice(QObject *parent):
    QIODevice(parent)
{

}
void audioDevice::refreshData(QByteArray data,int size){
    mData = data;
    dataSize = size;
}
qint64 audioDevice::readData(char *data, qint64 maxlen){
    //if(NULL == mData) return 0;
    /*
    if(dataSize > maxlen){
        memcpy(data, mData.data(), maxlen);
        mData.remove(0, int(maxlen));
        dataSize -= maxlen;
        return maxlen;
    }else{
        memcpy(data, mData.data(), dataSize);
        mData.clear();
        int tmp = dataSize;
        dataSize = 0;
        return  tmp;
    }*/
    if (mData.size() >= maxlen) {
           QByteArray d = mData.mid(0, int(maxlen));
           memcpy(data, d.data(), size_t(d.size()));
           mData.remove(0, int(maxlen));
           return d.size();
       } else {
           QByteArray d = mData;
           memcpy(data, d.data(), size_t(d.size()));
           mData.clear();
           return d.size();
       }

}
qint64 audioDevice::writeData(const char *data, qint64 len){


}
