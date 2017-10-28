#ifndef CL_SW_INFO_H
#define CL_SW_INFO_H

#include "ocl_h.h"

class cl_sw_info
{

public:
    cl_sw_info(bool doubleBufferEnable, cl_context context, cl_device_id device_id);
    cl_sw_info();
   void createOOEqueue();
   void CreateIOEqueue();
   void createMemBuff(void* data_buffer_i, void* data_buffer_o, void* data_trans,
                      size_t size_trans, size_t size_data_buffer);
   void writeToDevice();
   void kernelExecute(cl_kernel kernel, ulong particle_cnt);
   ulong  getEventDuration(cl_event event);
   void readFromDevice(ulong count);
   void setDeviceType(cl_device_type d_type);
   void kernelRunsim(ulong count);
private:

    bool doubleBuffer;
    cl_mem* memBuffer_i;
    cl_mem* memBuffer_o;
    cl_mem* memBuffer_trans;
    cl_context context;
    cl_command_queue cmd_queue;
    cl_device_type device_type;
    cl_kernel kernel;
    cl_device_id device_id;
    cl_int error;
    ulong particle_cnt;
    cl_event WriteEvent[2];
    cl_event exeEvent;
    cl_event ReadEvent;
    void* data_buffer_i;
    void* data_buffer_o;
    void* data_trans;
    size_t size_data_buffer;
    size_t size_trans;
    cl_int queue_config;
};

#endif // CL_SW_INFO_H
