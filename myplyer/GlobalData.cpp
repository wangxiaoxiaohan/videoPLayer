#include "GlobalData.h"

globalData* globalData::_instance = NULL;
globalData* globalData::getInstance(){
    if(_instance==NULL){
        _instance = new globalData();
    }
    return _instance;
}

globalData::globalData()
{
 g_GLList.clear();
}
