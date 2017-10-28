#ifndef PSTATEMACHINE_H
#define PSTATEMACHINE_H

#include <QThread>
#include <QtCore>
#include "cl_hw_info.h"
#include "particle.h"
#include "trans_mat.h"

enum class psmState {st_idle, st_pause, st_run, st_done, st_stop,
                    st_copy, st_error, st_check};

class pStateMachine : public
    QThread
{
   // Q_OBJECT
public:
  //  explicit pStateMachine(QWidget *parent=0);
   explicit pStateMachine();
   void run(); // override the run function in Qthread class.
   bool stop;
   psmState getCurrentState();
   void setParticleCount(ulong particle_cnt);
   void setCurrenState(psmState nextState);
   void populateDeviceInfo(cl_hw_info* taskSettings, cl_uint num_platforms);
   void create_test_bed(unsigned config);
   void mapDataToOcl();
   void checkDataOut();
private:
    psmState currentState;
    cl_hw_info *DeviceInfo;
    cl_uint num_platforms;
    ulong count;
    ulong particle_cnt;
    particle_s <float> *current_data_state;
    particle_s <float> *next_data_state;
    particle_s <float> *exp_data_state;
    particle_s <float> *intermed_state;
    trans_mat  <float> *trans_mat_cell;
  // void finished_work();
};

#endif // PSTATEMACHINE_H
