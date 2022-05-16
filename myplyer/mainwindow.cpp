 #include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qpushbutton.h"
#include "qfile.h"
#include "mediaplayer/player.h"
#include "glyuvwidget3.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setGeometry(0,0,1920,1080);
    glw = new glyuvwidget2(this);

    glw->setGeometry(0,0,1920,1080) ;
    glw->show();

   QPushButton *bt =new QPushButton(this);
   bt->setGeometry(200,200,50,50);
   bt->show();
    connect(bt,SIGNAL(clicked()),this,SLOT(btclicked()));

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::btclicked(){
    char path[64];
    sprintf(path,"/home/wanghan/test/100s.mp4");

    splayer *player = new  splayer(glw);
    player->prepare(QString(path));
    player->play();


}
