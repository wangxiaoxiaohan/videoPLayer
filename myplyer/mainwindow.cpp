#include "mainwindow.h"
#include "qfile.h"
#include "glyuvwidget3.h"
#include <QIcon>
#include <QPixmap>
#include <QProgressBar>
#include <QFileDialog>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{

    setGeometry(0,0,1920,1160);
    int iconW = 100;
    int iconH = 100;
    int margin = 10;


    glw = new glyuvwidget2(this);
    glw->setGeometry(0,0,1920,1080) ;
    glw->show();

    play_pause =new QPushButton(this);
    play_pause->setGeometry(0,1080 + margin * 3,50,50);
    play_pause->setIcon(QIcon(QPixmap(":/new/prefix1/images/play.png")));
    play_pause->setIconSize(QSize(iconW,iconH));
    play_pause->show();
    connect(play_pause,SIGNAL(clicked()),this,SLOT(pp_btclicked()));

    stop =new QPushButton(this);
    stop->setGeometry(50 + margin,1080 + margin * 3,50,50);
    stop->setIcon(QIcon(QPixmap(":/new/prefix1/images/stop.png")));
    stop->setIconSize(QSize(iconW,iconH));
    stop->show();
    connect(stop,SIGNAL(clicked()),this,SLOT(stop_btclicked()));

    open =new QPushButton(this);
    open->setGeometry((50 + margin) * 2,1080 + margin * 3,50,50);
    open->setIcon(QIcon(QPixmap(":/new/prefix1/images/file-open.png")));
    open->setIconSize(QSize(50,50));
    open->show();
    connect(open,SIGNAL(clicked()),this,SLOT(open_btclicked()));

    QProgressBar *progress = new QProgressBar(this);
    progress->setGeometry(margin,1080 + margin * 1,1920 - margin *2,10);
    progress->show();


    mPlayer = new  splayer(glw);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::pp_btclicked(){

    if(mPlayer->mediaStatus() == media_playing){
        mPlayer->pause();
        play_pause->setIcon(QIcon(QPixmap(":/new/prefix1/images/play.png")));
    }else{
        mPlayer->play();
        play_pause->setIcon(QIcon(QPixmap(":/new/prefix1/images/pause.png")));
    }


}
void MainWindow::open_btclicked(){
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), "/home/wanghan/test", tr("Image Files (*.mp4 *.mp3 *.bmp)"));
    if(!fileName.isNull())
        mPlayer->prepare(fileName);
   //

}
void MainWindow::stop_btclicked(){

    mPlayer->stop();

}
