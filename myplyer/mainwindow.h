#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include "glyuvwidget3.h"
#include "mediaplayer/player.h"

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
private:
    Ui::MainWindow *ui;
    glyuvwidget2 *glw;
    splayer* mPlayer;
    QPushButton *play_pause;
    QPushButton *stop;
    QPushButton *open;
};

#endif // MAINWINDOW_H
