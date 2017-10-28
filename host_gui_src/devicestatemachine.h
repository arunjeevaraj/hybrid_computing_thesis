#ifndef DEVICESTATEMACHINE_H
#define DEVICESTATEMACHINE_H

#include"ocl_h.h"

enum class State {st_ilde, st_run, st_done};

class DeviceStateMachine
{

public:
    DeviceStateMachine(cl_device_id device_id);
    State get_current_state();
    void state_machine_exec();
    void set_device_select(bool set_selection);
private:
    State current_state;
    bool device_selected;
    cl_device_id d_id;
};

#endif // DEVICESTATEMACHINE_H
