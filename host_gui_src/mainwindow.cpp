#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

#include "ocl_h.h"
#include "QString"

#include "cl_hw_info.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mPlatform()
{
    ocl_execution_done = false;

    ui->setupUi(this);
    connect(ui->helpButton, &QPushButton::clicked,
            this, &MainWindow::updateStatus);

    connect(ui->quit, &QPushButton::clicked,
            QApplication::instance(), &QApplication::quit);

    connect(ui->startSim, &QPushButton::clicked,
            this, &MainWindow::startSim);

    // openCl device detection and registeration.
    int err = clGetPlatformIDs(0, NULL, &num_platforms);
    if (err != CL_SUCCESS) {
             qDebug() << "ocl error" ;
    }

    device_list* device;
    char query_str[50];
    cl_platform_id* platform_list;
    platform_list = new cl_platform_id[num_platforms];
    clGetPlatformIDs(num_platforms, platform_list, NULL);
    QString platform_name;
    for (unsigned i = 0; i < num_platforms ; i++) {
        clGetPlatformInfo(platform_list[i], CL_PLATFORM_NAME, sizeof(query_str), &query_str, NULL);
        platform_name = QString::fromUtf8(query_str);
        device = new device_list(platform_name, platform_list[i]);
        mPlatform.append(device);
        ui->deviceLayout->addWidget(device);
        //qDebug() << platform_name;
    }
    ui->particleCount->setText("16777216");
   // QString part_str = ui->particleCount->text();
   // ulong part_cnt = part_str.toULong();
   // qDebug() << "particle count" << part_cnt;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow:: updateStatus()
{
    qDebug() << "Help button clicked" ;
    //qDebug() << QString("Number of platforms : %1 ").arg(num_platforms);
}

void MainWindow:: startSim()
{
    unsigned num_platforms_selected = 0;
    for (unsigned i = 0; i < num_platforms ; i++ ) {
        if(mPlatform[i]->isSelected()) {
            num_platforms_selected++;
        }
    }
    if(num_platforms_selected) {
        cl_hw_info* hw_info_container = new cl_hw_info[num_platforms_selected];
        //make a copy of mplatform objects.
        unsigned k = 0;
        for (unsigned i = 0; i < num_platforms ; i++ ) {
            if(mPlatform[i]->isSelected()) {
                qDebug() << mPlatform[i]->getSelectedDevices();
                cl_hw_info copy_sim_hw_info(mPlatform[i]->getSelectedDevices(), mPlatform[i]->getPlatformId());
                copy_sim_hw_info.SetDeviceId(mPlatform[i]->getSelectedDevicesIds());
                hw_info_container[k]= copy_sim_hw_info;
                k++;
            }
        }
        qDebug() << "copying hw info to the task object is done";

        //send the copy to mthread object.
        //extract the settings to the thread.
        //execute the ocl on that thread.
        //
        sim_task* new_task;
        ulong particle_cnt = ui->particleCount->text().toULong();
        new_task = new sim_task(hw_info_container, particle_cnt,
                                num_platforms_selected);
        ui->simSetting->addWidget(new_task);
        mSimTask.append(new_task);

       /*
        bool* done = new bool[num_platforms];
        unsigned all_platform_done = 0;
         qDebug() << "Start sim was clicked" ;
         //entry point for the statemachine.
        while (all_platform_done<num_platforms) {
                all_platform_done = 0;
            for(unsigned i = 0; i < num_platforms ; i++) {
                done[i] = mPlatform[i]->run_platform();
                all_platform_done += done[i];
            }
        }*/
    } else  {
        qDebug() << "No platforms were selected." ;
    }
}
