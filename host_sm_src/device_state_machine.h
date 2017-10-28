/*******************************************************************************
// Copyright (C) 2017
// Author: Arun Jeevaraj.
// All rights reserved.
*******************************************************************************/
#ifndef DEVICE_STATE_MACHINE_H
#define DEVICE_STATE_MACHINE_H

#include "CL/cl.h"
#include "cl_sw_info.h"
#include <QThread>
#include <QtCore>
#include <stdint.h>

enum class device_states { st_idle, st_initialized, st_ready_to_create_mem , st_mem_attached, st_load_tm,
    st_execute_ping, st_execute_pong, st_execute_done, st_device_done, st_device_continue, st_execute_ready,
    st_read_device, st_copy_data_settings, st_read_by_host};      // the states of the state machine.
enum class device_config { device_single_buffer, device_double_buffer};
// device configuration.


class device_state_machine : public
        QThread
{
public:
    // constructor to attach the d_id , p_id and device_config.
    device_state_machine(cl_device_id d_id, cl_platform_id p_id, device_config d_conf);
    // to set the particle count, used to make the NDRANGE space for kernel execution.
    void set_particle_cnt(uint64_t particle_cnt);
    // get the current state
    device_states get_current_state();
    // change the state of the stateMachine.
    void state_change_command(device_states command);

    //set the frame starting address and size.
    void set_frame_data(void *frame_ping, void *frame_pong, uint64_t size );
    // used to select the current transfer matrix for the kernel execution.
    void set_current_tm(unsigned tm_id);
    // set the pointers for the tm data to be used to create cl mem objects.
    void set_transfer_mat(void *focus, void *drift, void *defocu, uint64_t size);
    // state device initialize function.
    device_states device_initialize(); // return st_initialized when done.
    // create the memory objects
    device_states device_create_mem_obejcts();
    // to load frame data to the device memory and transfer matrix memory objects to device memory.
    void write_to_device_memory();
    // kernel execution with ping as input frame and pong as output frame.
    void kernel_execute_ping_pong();
    // kernel execution with pong as input frame and ping as input frame.
    void kernel_execute_pong_ping();
    // sets the frame count for the simulation.
    void set_frame_count(uint64_t frame_count);
    void run();
private:
    device_states current_states;
    device_states mt_state_change;
    device_config device_configuration;
    cl_device_id d_id;
    cl_platform_id p_id;
    bool double_buff;
    void *frame_ping;
    void *frame_pong;
    uint64_t frame_size;
    void *tm_focus;
    void *tm_drift;
    void *tm_refocus;
    uint64_t tm_size;
    uint64_t particle_count;
    uint64_t frame_count;
    bool ping_is_output;
    cl_sw_info ocl_sw;
    unsigned tm_id;
};

#endif // DEVICE_STATE_MACHINE_H
