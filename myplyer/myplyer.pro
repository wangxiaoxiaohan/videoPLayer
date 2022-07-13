#-------------------------------------------------
#
# Project created by QtCreator 2022-03-21T22:12:02
#
#-------------------------------------------------

QT       += core gui
QT       += opengl
QT       += multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = myplyer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    GlobalData.cpp \
    decoder/decode.cpp \
    demuxer/demux.cpp \
    base/framequeue.cpp \
    mediaplayer/player.cpp \
    base/packetqueue.cpp \
    glyuvwidget3.cpp \
    audiooutput.cpp \
    audiodevice.cpp \
    widget/mediatime.cpp \
    widget/myslider.cpp






HEADERS += \
        mainwindow.h \
    GlobalData.h \
    mainwindow.h \
    base/framequeue.h \
    base/packetqueue.h \
    decoder/decode.h \
    mediaplayer/playlist.h \
    demuxer/demux.h \
    mediaplayer/player.h \
    glyuvwidget3.h \
    audiooutput.h \
    audiodevice.h \
    widget/mediatime.h \
    widget/myslider.h


FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../ffmpeg_install_dir/lib/release/ -lavcodec
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../ffmpeg_install_dir/lib/debug/ -lavcodec
else:unix: LIBS += -L$$PWD/../ffmpeg_install_dir/lib/ -lavutil -lavcodec -lavformat  -lswscale -lavdevice -lswresample

INCLUDEPATH += $$PWD/../ffmpeg_install_dir/include
DEPENDPATH += $$PWD/../ffmpeg_install_dir/include

RESOURCES += \
    images.qrc
