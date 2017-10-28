#include "devicestatemachine.h"
#include <QDebug>

DeviceStateMachine::DeviceStateMachine(cl_device_id device_id)
{
    d_id = device_id;
    current_state = State::st_ilde;
}


State DeviceStateMachine::get_current_state()
{
    return current_state;
}
void DeviceStateMachine::state_machine_exec()
{

}

void DeviceStateMachine::set_device_select(bool set_selection)
{
    device_selected = set_selection;
    qDebug() << set_selection << "device_selected";
}
