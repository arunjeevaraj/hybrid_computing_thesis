/*******************************************************************************
// Copyright (C) 2017
// Author: Arun Jeevaraj.
// All rights reserved.
*******************************************************************************/
#include "device_state_machine.h"


device_state_machine::device_state_machine(cl_device_id d_id, cl_platform_id p_id, device_config d_conf)
{
    QMutex mutex;
    mutex.lock();
    this->device_configuration = d_conf;
    this->current_states = device_states::st_idle;
    this->d_id = d_id;
    this->p_id = p_id;
    //std::cout<< "device_state_machine copying settings from main thread..."<< std::endl;
    if (d_conf == device_config::device_double_buffer) {
        this->double_buff = true;
       // ocl_sw.set_device_info(d_id, p_id, true);
    } else {
        this->double_buff = false;
    }
    mutex.unlock();
       // ocl_sw.set_device_info(d_id, p_id, false);
}


void device_state_machine::run()
{
    //std::cout<< "device_state_machine thread started"<< std::endl;
    // this is like the thread main function.
    QMutex mutex;
    device_states mt_cs; // main thread cs.
    device_states d_cs;  // current state.
    device_states d_ns;  // next state.
    unsigned current_tm_set; // to be used inside the device thread.
    cl_sw_info ocl_sw_i_thread;// ocl structure inside the thread.
    uint64_t frame_count_max, frame_i;
    frame_i = 0;
    // all the above constructs is for variable sharing, between the threads.
    // step 1 initialize the device, and build the opencl kernel for the
    // device context.
    unsigned f_tm_sel;
    // d_ns = device_initialize();
    bool ping_is_output = false;
    mutex.lock();
    frame_count_max = this->frame_count;
    ocl_sw_i_thread.set_device_info(d_id, p_id, double_buff);
    ocl_sw_i_thread.get_device_kernel("kcpu.cl", "kernelCPU1", true);
    mutex.unlock();
    d_ns = device_states::st_initialized;
    while (1) {
        mutex.lock();
        mt_cs = mt_state_change;
        mt_state_change = device_states::st_idle;
        current_states = d_ns;
        d_cs = current_states;
        //current_tm_set = this->tm_id;
        mutex.unlock();


        if (mt_cs == device_states::st_copy_data_settings) {
            //std::cout << "copy data settings" << std::endl;
            mutex.lock();
            ocl_sw_i_thread.set_frame_data(frame_ping, frame_pong, frame_size);
            ocl_sw_i_thread.set_particle_cnt(particle_count);
            ocl_sw_i_thread.set_tm_data(tm_focus, tm_drift, tm_refocus, tm_size);
            mutex.unlock();
            d_ns = device_states::st_ready_to_create_mem;
        } else if (mt_cs == device_states::st_load_tm) {
            d_ns = device_states::st_load_tm;
        } else if (mt_cs == device_states::st_read_device) {
            ocl_sw_i_thread.read_device_data(ping_is_output); // read based on last device execution.
            d_ns = device_states::st_device_done;
        }

        if (d_cs == device_states::st_ready_to_create_mem) {
            ocl_sw_i_thread.create_mem_objects();
            d_ns = device_states::st_load_tm;
        } else if (d_cs == device_states::st_execute_ping) {
            ocl_sw_i_thread.kernel_execture_ping_pong();
            ocl_sw_i_thread.check_command_queue();
            frame_i++;
             ping_is_output = true;
            d_ns = device_states::st_execute_done;
         } else if (d_cs == device_states::st_execute_pong) {
            ocl_sw_i_thread.kernel_execture_pong_ping();
            ocl_sw_i_thread.check_command_queue();
            frame_i++;
            ping_is_output = false;
            d_ns = device_states::st_execute_done;

        } else if(d_cs == device_states::st_device_done) {

            //std::cout << "thread work done" << std::endl;
            break;
        }  else if (d_cs == device_states::st_load_tm) {
            f_tm_sel = frame_i % 5;
            if (f_tm_sel == 0 || f_tm_sel == 4) {
                current_tm_set = 0;
            } else if(f_tm_sel == 1 || f_tm_sel == 3) {
                current_tm_set = 1;
            } else {
                current_tm_set = 2;
            }
            //std::cout << "setting tm: " << current_tm_set << std::endl;
            ocl_sw_i_thread.set_current_tm(current_tm_set);
            d_ns = device_states::st_execute_ready;
        } else if (d_cs == device_states::st_execute_ready) {
            if (ping_is_output) {
                d_ns = device_states::st_execute_pong;
            } else {
                d_ns = device_states::st_execute_ping;
            }
        }
    }
}

device_states device_state_machine::device_initialize()
{
    // get the device opencl structures setup.
    ocl_sw.get_device_kernel("kcpu.cl", "kernelCPU1", true);
    //std::cout << "--------st_Device initialized\n";
    return  device_states::st_initialized;
}

device_states device_state_machine::get_current_state()
{

    QMutex mutex;
    mutex.lock();
    device_states return_state = this->current_states;
    this->current_states = device_states::st_read_by_host;
    mutex.unlock();
        return return_state;

}

void device_state_machine::state_change_command(device_states command)
{
    QMutex mutex;
    mutex.lock();
    this->mt_state_change = command;
    mutex.unlock();
}

void device_state_machine::set_frame_data(void *frame_ping, void *frame_pong, uint64_t size)
{
    //std::cout << "setting the frame data pointer" << std::endl;
    QMutex mutex;
    mutex.lock();
    this->frame_ping = frame_ping;
    this->frame_pong = frame_pong;
    this->frame_size = size;
    mutex.unlock();
    //ocl_sw.set_frame_data(frame_ping, frame_pong, size);
}

void device_state_machine::set_current_tm(unsigned tm_id)
{
    QMutex mutex;
    mutex.lock();
    this->tm_id = tm_id;
    this->mt_state_change = device_states::st_load_tm;
    mutex.unlock();
}

device_states device_state_machine::device_create_mem_obejcts()
{
    ocl_sw.create_mem_objects();
    //std::cout << "--------st_mem_attached\n";
    return device_states::st_mem_attached;
}

void device_state_machine::write_to_device_memory()
{
    //std::cout << "writing to device memory" << std::endl;
    ocl_sw.writeToDevice();
    this->current_states = device_states::st_execute_done;
}

void device_state_machine::kernel_execute_ping_pong()
{
    ping_is_output = false;
    //std::cout << " kernel execution ping pong " << std::endl;
    ocl_sw.kernel_execture_ping_pong();
}

void device_state_machine::kernel_execute_pong_ping()
{
    ping_is_output = true;
    //std::cout << " kernel execution pong ping" << std::endl;
    ocl_sw.kernel_execture_pong_ping();
}

void device_state_machine::set_particle_cnt(uint64_t particle_cnt)
{
    QMutex mutex;
    mutex.lock();
    this->particle_count = particle_cnt;
    mutex.unlock();
    //ocl_sw.set_particle_cnt(particle_cnt);
}

void device_state_machine::set_transfer_mat(void *focus, void *drift, void *defocu, uint64_t size)
{
    QMutex mutex;
    mutex.lock();
    this->tm_focus = focus;
    this->tm_drift = drift;
    this->tm_refocus = defocu;
    this->tm_size = size;
    mutex.unlock();
    //ocl_sw.set_tm_data(focus, drift, defocu, size);
}

void device_state_machine::set_frame_count(uint64_t frame_count)
{
    QMutex mutex;
    mutex.lock();
    this->frame_count = frame_count;
    mutex.unlock();
}
