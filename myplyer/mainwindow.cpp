#include "mainwindow.h"
#include "qfile.h"
#include "glyuvwidget3.h"
#include <QIcon>
#include <QPixmap>
#include <QFileDialog>
#include <QTimeEdit>


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

    int timeWidth = 88;
    played = new MediaTime(this);
    played->setGeometry(0,1080 + margin * 1,timeWidth,15);


    slider = new mySlider(this);
    slider->setGeometry(margin + timeWidth,1080 + margin * 1,1920 - margin * 2 - timeWidth,25);
    slider->setRange(0,1000);
    slider->setMinimum(0);
    slider->setMaximum(1000);
    slider->setOrientation(Qt::Horizontal);
    slider->setStyleSheet(
    "QSlider::groove:horizontal{height:2px;background-color:rgb(219,219,219);}\
    QSlider::handle:horizontal{width:16px;background-color:rgb(128,128,128);margin:-7px 0px -7px 0px;border-radius:8px;}\
    QSlider::add-page:horizontal{background-color:rgb(219,219,219);}\
    QSlider::sub-page:horizontal{background-color:rgb(26,217,110);}"

    );

    connect(slider,SIGNAL(releaseProgress(double)),this,SLOT(progressChanged(double)));



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

    decreaseSpeed = new QPushButton(this);
    decreaseSpeed->setGeometry((50 + margin) * 3,1080 + margin * 3,50,50);
    decreaseSpeed->setIcon(QIcon(QPixmap(":/new/prefix1/images/decreaseSpeed.png")));
    decreaseSpeed->setIconSize(QSize(50,50));
    connect(decreaseSpeed,SIGNAL(clicked()),this,SLOT(decreaseSpeedSlot()));

    playSpeedLabel = new QLabel(this);
    playSpeedLabel->setGeometry((50 + margin) * 4,1080 + margin * 3,50,50);
    playSpeedLabel->setAlignment(Qt::AlignCenter);

    increaseSpeed = new QPushButton(this);
    increaseSpeed->setGeometry((50 + margin) * 5,1080 + margin * 3,50,50);
    increaseSpeed->setIcon(QIcon(QPixmap(":/new/prefix1/images/increaseSpeed.png")));
    increaseSpeed->setIconSize(QSize(50,50));
    connect(increaseSpeed,SIGNAL(clicked()),this,SLOT(increaseSpeedSlot()));
    totalTime = new MediaTime(this);
    totalTime->setGeometry(1920 - margin * 2 - timeWidth,1080 + margin * 3,timeWidth,15);

    mPlayer = new  splayer(glw);
    connect(mPlayer,SIGNAL(updateProgress(int,int)),this,SLOT(updateProgress(int,int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::updateProgress(int current,int total){
    played->setTime(current);
    totalTime->setTime(total);
    double value = ((double)current / (double)total) * 1000;
    //qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< value;
    if(!slider->pressed)
        slider->setValue(value);
}
void MainWindow::pp_btclicked(){
    if(mPlayer->mediaStatus() == media_stopped)
        return;
    if(mPlayer->mediaStatus() == media_playing){
        mPlayer->pause();
        play_pause->setIcon(QIcon(QPixmap(":/new/prefix1/images/play.png")));
    }else{
        mPlayer->play();
        play_pause->setIcon(QIcon(QPixmap(":/new/prefix1/images/pause.png")));
    }


}
void MainWindow::open_btclicked(){
    if(mPlayer->mediaStatus() == media_playing)
        return;
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), "/home/wanghan/test", tr("Image Files (*.mp4 *.mp3 *.bmp)"));
    if(!fileName.isNull()){
        mPlayer->prepare(fileName);
        play_pause->setIcon(QIcon(QPixmap(":/new/prefix1/images/play.png")));
    }


}
void MainWindow::stop_btclicked(){

    mPlayer->stop();
    play_pause->setIcon(QIcon(QPixmap(":/new/prefix1/images/play.png")));
}
void MainWindow::increaseSpeedSlot(){
    mPlayer->setPlayRate(mPlayer->playRate() + 0.1);
    playSpeedLabel->setText(QString::number(mPlayer->playRate()));
}
void MainWindow::decreaseSpeedSlot(){
    mPlayer->setPlayRate(mPlayer->playRate() - 0.1);
    playSpeedLabel->setText(QString::number(mPlayer->playRate()));
}
void MainWindow::progressChanged(double value){

     mPlayer->seek(value);

}
