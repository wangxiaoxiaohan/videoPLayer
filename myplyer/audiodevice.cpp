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
    if(dataSize > maxlen){
        memcpy(data, mData.data(), maxlen);
        mData += maxlen;
        dataSize -= maxlen;
        return maxlen;
    }else{
        memcpy(data, mData.data(), dataSize);
        mData = NULL;
        int tmp = dataSize;
        dataSize = 0;
        return  tmp;
    }

}
qint64 audioDevice::writeData(const char *data, qint64 len){


}
