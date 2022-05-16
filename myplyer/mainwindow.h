#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "glyuvwidget3.h"
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
    void btclicked();

private:
    Ui::MainWindow *ui;
    glyuvwidget2 *glw;
};

#endif // MAINWINDOW_H
