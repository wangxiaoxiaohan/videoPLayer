#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QProgressBar>
#include "glyuvwidget3.h"
#include "mediaplayer/player.h"
#include "widget/mediatime.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void pp_btclicked();
    void open_btclicked();
    void stop_btclicked();
    void updateProgress(int current,int total);
private:
    Ui::MainWindow *ui;
    glyuvwidget2 *glw;
    splayer* mPlayer;
    QPushButton *play_pause;
    QPushButton *stop;
    QPushButton *open;
    MediaTime *played;
    MediaTime *totalTime;
    QProgressBar *progress;
};

#endif // MAINWINDOW_H
