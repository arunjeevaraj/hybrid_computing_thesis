/*******************************************************************************
// Copyright (C) 2017
// Author: Arun Jeevaraj.
// All rights reserved.
*******************************************************************************/
#ifndef CL_SW_INFO_H
#define CL_SW_INFO_H

#include "CL/cl.h"
#include <iostream>

class cl_sw_info
{

public:
   // empty constructor.
   cl_sw_info();
   // attach the device id and platform id of the target.
   // also finds the device type, creates a context, and a command queue.
   void set_device_info(cl_device_id d_id, cl_platform_id p_id, bool double_buffer);
   // compiles the kernel source, or attaches the kernel binary based on the online_compile
   void get_device_kernel(const char *kernel_file, const char *kernel_name, bool online_compile);
   // set the frame data ping and pong.
   void set_frame_data(void *data_i, void *data_o, uint64_t buffer_size);
   // set the transfer matrix
   void set_tm_data(void *data_tm_rfocus, void *data_tm_rdrift, void *data_tm_rdefocus, uint64_t buffer_size);
   // creates the memory objects for the OpenCL
   void create_mem_objects();
   // write to the device memory. only the frame data and transfer matrix.
   void writeToDevice();
    // kernel execute ping pong , links the memory object to the kernel, and perform kernel execution.
   void kernel_execture_ping_pong();
    // to set the particle count, used for NDRange execution space definition.
   void set_particle_cnt(uint64_t particle_cnt);
   // to check if the command queue has completed.
   void check_command_queue(); // blocks the thread there.
   // kernel execute pong ping, links the memory object to the kernel an perform kernel execution.
    void kernel_execture_pong_ping();
    // read the device memory to the host memory.
   void read_device_data(bool ping);
    // sets the current tm
    void set_current_tm(unsigned tm_id);
   // used to check the error status.
   void check_device_status(const char* log_message);
   // deep copy the settings to the thread memory.
   //void deepcopy(cl_sw_info src_copy);
private:

    bool doubleBuffer;
    cl_mem* memBuffer_ping;
    cl_mem* memBuffer_pong;
    cl_mem* memBuffer_trans;
    cl_context context;
    cl_command_queue cmd_queue;
    cl_device_type device_type;
    cl_kernel kernel;
    cl_device_id device_id;
    cl_platform_id platform_id;
    cl_int error;
    uint64_t particle_cnt;
    cl_event WriteEvent[2];
    cl_event exeEvent;
    cl_event ReadEvent;
    void* data_buffer_ping;
    void* data_buffer_pong;
    void* data_trans_drift;
    void* data_trans_focus;
    void* data_trans_defocus;
    size_t size_data_buffer;
    size_t size_trans;
    cl_int queue_config;
    unsigned tm_id;
};


#endif // CL_SW_INFO_H
