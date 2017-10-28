#include "sim_task.h"
#include "ui_sim_task.h"
#include <QDebug>
#include <QString>

const unsigned defaultTestConfig = 0;

sim_task::sim_task(cl_hw_info *taskSettings, ulong particle_cnt, cl_uint num_platforms,
                    QWidget *parent) :
    QWidget(parent),
    ui(new Ui::sim_task)
{
    ui->setupUi(this);
    this->particle_cnt = particle_cnt;
    this->num_platforms = num_platforms;
    this->taskSettings = new cl_hw_info[num_platforms];
    for (unsigned j = 0; j< num_platforms ; j++) {
        this->taskSettings[j] = taskSettings [j];
    }

    QString deviceName;
    char queryStr[50];
        qDebug() << QString("num of platforms found %1 ").arg(num_platforms);
    for (unsigned j = 0; j< num_platforms ; j++) {
        for (unsigned i = 0 ; i < this->taskSettings[j].getNumberDevices() ; i++) {
        clGetDeviceInfo(this->taskSettings[j].getDeviceId(i), CL_DEVICE_NAME, sizeof(queryStr),
             &queryStr, NULL);
        deviceName = QString::fromUtf8(queryStr);
        qDebug() << "device:" ;
        qDebug() << deviceName ;
        }
    }

    this->run_task();
    refreshTimer.setInterval(200);
    connect(&refreshTimer, &QTimer::timeout,
            this, &sim_task::update_status);
    QTimer::singleShot(200,[this] {refreshTimer.start();} );
    //new_taskthread = new pStateMachine;
}

sim_task::~sim_task()
{
    delete ui;
}

void sim_task::run_task()
{
    new_taskthread.setParticleCount(this->particle_cnt);
    new_taskthread.populateDeviceInfo(taskSettings, num_platforms);
    new_taskthread.create_test_bed(defaultTestConfig);
    new_taskthread.mapDataToOcl();
    new_taskthread.start();
}

void sim_task ::update_status()
{
    ThreadState = new_taskthread.getCurrentState();
    if(ThreadState == psmState::st_done) {
        qDebug() << "thread work done" ;//<< ThreadState;
        this->~sim_task();
    } else if(ThreadState == psmState::st_check) {
        qDebug() << "checking if test is valid." << endl;
        new_taskthread.checkDataOut();
    }
}

void sim_task:: stop_task()
{

}

void sim_task:: pause_task()
{

}
