#ifndef SIM_TASK_H
#define SIM_TASK_H

#include <QWidget>
#include "cl_hw_info.h"
#include "pstatemachine.h"
#include <QTimer>

namespace Ui {
class sim_task;
}

class sim_task : public QWidget
{
    Q_OBJECT

public:
    explicit sim_task(cl_hw_info *taskSettings, ulong particle_cnt,
        cl_uint num_platforms,
        QWidget *parent = 0);
    ~sim_task();
    void run_task();

public slots:
    void update_status();
    void stop_task();
    void pause_task();
signals:
    void taskDeleteSignal();

private:
    Ui::sim_task *ui;
    cl_hw_info *taskSettings;
    cl_uint num_platforms;
    ulong particle_cnt;
    pStateMachine new_taskthread;
    QTimer refreshTimer;
    psmState ThreadState;
};

#endif // SIM_TASK_H
