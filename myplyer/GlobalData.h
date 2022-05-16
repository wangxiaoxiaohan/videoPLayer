#ifndef GLOBALDATA_H
#define GLOBALDATA_H
#include "glyuvwidget2.h"
class globalData
{

    private:
        globalData();
        static globalData* _instance;

    public:
        static globalData* getInstance();
    public:
        QList<GLuint> g_GLList;//回收利用纹理

};
#endif // GLOBALDATA_H
