#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include "device_list.h"
#include <vector>
#include "cl_hw_info.h"
#include "sim_task.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void updateStatus();
    void startSim();
private:
    cl_uint num_platforms;
    Ui::MainWindow *ui;
    QVector<device_list*> mPlatform;
    bool ocl_execution_done;
    QVector<sim_task*> mSimTask;
};

#endif // MAINWINDOW_H
