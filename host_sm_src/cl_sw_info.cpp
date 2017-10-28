/*******************************************************************************
// Copyright (C) 2017
// Author: Arun Jeevaraj.
// All rights reserved.
*******************************************************************************/
#include "cl_sw_info.h"
#include <stdio.h>

void read_src_file(const char *file_name, char **source, size_t *src_size);


cl_sw_info::cl_sw_info ()
{
// an empty constructor.
}

void cl_sw_info::set_device_info(cl_device_id d_id, cl_platform_id p_id, bool double_buffer)
{
    this->device_id = d_id;
    this->platform_id = p_id;
    this->device_type = CL_DEVICE_TYPE_DEFAULT;
    clGetDeviceInfo(d_id, CL_DEVICE_TYPE, sizeof(this->device_type), &device_type, 0);
    this->context = clCreateContext(NULL, 1, &this->device_id, NULL, NULL, &error);
    check_device_status("creating context..");
    this->cmd_queue = clCreateCommandQueue(this->context, this->device_id, 0, NULL); // no profiling enabled.
    check_device_status("creating commanding queue..");
    this->doubleBuffer = double_buffer;
}


void cl_sw_info::get_device_kernel(const char *kernel_file, const char *kernel_name, bool online_compile)
{
    if (online_compile == 1) {
        char* src_kernel;
        size_t src_size;
        read_src_file(kernel_file, &src_kernel, &src_size);
        cl_program program = clCreateProgramWithSource(this->context, 1, (const char**)&src_kernel, &src_size, &error);
        check_device_status("creating program inside the context.");
        error = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
        check_device_status("building the program");
        this->kernel = clCreateKernel(program, kernel_name, &error);
        check_device_status("creating kernel for device");
    } else { // do offline compile. Target: FPGA.

    }
}

void cl_sw_info::set_frame_data(void *data_i, void *data_o, uint64_t buffer_size)
{
    this->data_buffer_ping = data_i;
    this->data_buffer_pong = data_o;
    this->size_data_buffer = buffer_size;
}

void cl_sw_info::set_tm_data(void *data_tm_rfocus, void *data_tm_rdrift, void *data_tm_rdefocus, uint64_t buffer_size)
{
    this->data_trans_focus = data_tm_rfocus;
    this->data_trans_drift = data_tm_rdrift;
    this->data_trans_defocus = data_tm_rdefocus;
    this->size_trans = buffer_size;
}

void cl_sw_info::create_mem_objects()
{
    if (this->doubleBuffer) {
        this->memBuffer_ping = new cl_mem[2];
        this->memBuffer_pong = new cl_mem[2];
        this->memBuffer_trans = new cl_mem[3];
        // not useful for cpu implementation, host memory and device memory are the same here.
    } else { // make a single buffer implementation.
        this->memBuffer_ping = new cl_mem[1];
        this->memBuffer_pong = new cl_mem[1];
        this->memBuffer_trans = new cl_mem[3];
        memBuffer_ping[0] = clCreateBuffer(this->context, CL_MEM_USE_HOST_PTR, this->size_data_buffer,
            data_buffer_ping, &error);
        check_device_status("creating memory buffer for ping");
        memBuffer_pong[0] = clCreateBuffer(this->context, CL_MEM_USE_HOST_PTR, size_data_buffer,
            data_buffer_pong, &error);
        check_device_status("creating memory buffer for pong");
        memBuffer_trans[0] = clCreateBuffer(this->context, CL_MEM_USE_HOST_PTR, size_trans, data_trans_focus, &error);
        check_device_status("creating memory buffer for focus transfer matrix.");
        memBuffer_trans[1] = clCreateBuffer(this->context, CL_MEM_USE_HOST_PTR, size_trans, data_trans_drift, &error);
        check_device_status("creating memory buffer for transfer matrix.");
        memBuffer_trans[2] = clCreateBuffer(this->context, CL_MEM_USE_HOST_PTR, size_trans, data_trans_defocus, &error);
        check_device_status("creating memory buffer for transfer matrix.");
    }

}

void cl_sw_info::writeToDevice()
{
        //std::cout << "writing through inorder queue. \n";
        error = clEnqueueMigrateMemObjects(cmd_queue, 1, memBuffer_ping, 0, 0, NULL, &WriteEvent[0] );
        clFinish(cmd_queue);
        check_device_status("loading frame data to device memory");
        error = clEnqueueMigrateMemObjects(cmd_queue, 1, &memBuffer_trans[0], 0, 0, NULL, &WriteEvent[1] );
        clFinish(cmd_queue);
        check_device_status("loading tm focus to device memory");
        error = clEnqueueMigrateMemObjects(cmd_queue, 1, &memBuffer_trans[1], 0, 0, NULL, &WriteEvent[1] );
        clFinish(cmd_queue);
        check_device_status("loading tm drift to device memory");
        error = clEnqueueMigrateMemObjects(cmd_queue, 1, &memBuffer_trans[2], 0, 0, NULL, &WriteEvent[1] );
        clFinish(cmd_queue);
        check_device_status("loading tm defocus to device memory");
}

void cl_sw_info::kernel_execture_ping_pong()
{
    //std::cout << "linking kernel arguments" << std::endl;
    //std::cout << "tm_id" << tm_id << std::endl;
    // linking ping as input frame.
    error = clSetKernelArg(kernel, 0, sizeof(cl_mem), memBuffer_ping);
    check_device_status("setting ping as input frame");
    error = clSetKernelArg(kernel, 1, sizeof(cl_mem), &memBuffer_trans[tm_id]);
    check_device_status("setting transfer matrix");
    error = clSetKernelArg(kernel, 2, sizeof(cl_mem), memBuffer_pong);
    check_device_status("setting pong as output frame");
    if (device_type == CL_DEVICE_TYPE_CPU) {
        //std::cout << "kernel execute ping to pong" << std::endl;
        size_t global_size[] = {1, 1, particle_cnt/16};
        error = clEnqueueNDRangeKernel(cmd_queue, kernel, 3, NULL, global_size, 0, 0, NULL, &exeEvent);
        check_device_status("kernel execution ");
    }
}
void cl_sw_info::set_particle_cnt(uint64_t particle_cnt)
{
    this->particle_cnt = particle_cnt;
}

void cl_sw_info::check_command_queue()
{
    clFinish(cmd_queue); // blocked here till command queue is finished.
}

void cl_sw_info::kernel_execture_pong_ping()
{
    //std::cout << "linking kernel arguments" << std::endl;
    //std::cout << "tm_id" << tm_id << std::endl;
    error = clSetKernelArg(kernel, 0, sizeof(cl_mem), memBuffer_pong);
    check_device_status("setting pong as input frame");
    error = clSetKernelArg(kernel, 1, sizeof(cl_mem), &memBuffer_trans[tm_id]);
    check_device_status("setting transfer matrix");
    error = clSetKernelArg(kernel, 2, sizeof(cl_mem), memBuffer_ping);
    check_device_status("setting ping as output frame");
    if (device_type == CL_DEVICE_TYPE_CPU) {
        //std::cout << "kernel execute pong to ping" << std::endl;
        size_t global_size[] = {1, 1, particle_cnt/16};
        error = clEnqueueNDRangeKernel(cmd_queue, kernel, 3, NULL, global_size, 0, 0, NULL, &exeEvent);
        check_device_status("kernel execution ");
    }
}

void cl_sw_info::read_device_data(bool ping)
{

    if (!ping) {
        // read the ping frame
        clEnqueueReadBuffer(cmd_queue, memBuffer_ping[0], CL_TRUE, 0, size_data_buffer,
            data_buffer_ping, 0, NULL, &ReadEvent);
        clFinish(cmd_queue);
         //std::cout << "Reading ping" << std::endl;
    } else {
        // read the pong frame
        //std::cout << "reading from pong mem to data buffer." << std::endl;
        clEnqueueReadBuffer(cmd_queue, memBuffer_pong[0], CL_TRUE, 0, size_data_buffer,
            data_buffer_pong, 0, NULL, &ReadEvent);
        clFinish(cmd_queue);
        //std::cout << "Reading pong" << std::endl;
    }
}

void cl_sw_info::set_current_tm(unsigned tm_id)
{
    this->tm_id = tm_id;
}

const char* TranslateOpenCLError(cl_int errorCode)
{
    switch (errorCode)
    {
    case CL_SUCCESS:                            return "CL_SUCCESS";
    case CL_DEVICE_NOT_FOUND:                   return "CL_DEVICE_NOT_FOUND";
    case CL_DEVICE_NOT_AVAILABLE:               return "CL_DEVICE_NOT_AVAILABLE";
    case CL_COMPILER_NOT_AVAILABLE:             return "CL_COMPILER_NOT_AVAILABLE";
    case CL_MEM_OBJECT_ALLOCATION_FAILURE:      return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
    case CL_OUT_OF_RESOURCES:                   return "CL_OUT_OF_RESOURCES";
    case CL_OUT_OF_HOST_MEMORY:                 return "CL_OUT_OF_HOST_MEMORY";
    case CL_PROFILING_INFO_NOT_AVAILABLE:       return "CL_PROFILING_INFO_NOT_AVAILABLE";
    case CL_MEM_COPY_OVERLAP:                   return "CL_MEM_COPY_OVERLAP";
    case CL_IMAGE_FORMAT_MISMATCH:              return "CL_IMAGE_FORMAT_MISMATCH";
    case CL_IMAGE_FORMAT_NOT_SUPPORTED:         return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
    case CL_BUILD_PROGRAM_FAILURE:              return "CL_BUILD_PROGRAM_FAILURE";
    case CL_MAP_FAILURE:                        return "CL_MAP_FAILURE";
    case CL_MISALIGNED_SUB_BUFFER_OFFSET:       return "CL_MISALIGNED_SUB_BUFFER_OFFSET";                          //-13
    case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:    return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";   //-14
    case CL_COMPILE_PROGRAM_FAILURE:            return "CL_COMPILE_PROGRAM_FAILURE";                               //-15
    case CL_LINKER_NOT_AVAILABLE:               return "CL_LINKER_NOT_AVAILABLE";                                  //-16
    case CL_LINK_PROGRAM_FAILURE:               return "CL_LINK_PROGRAM_FAILURE";                                  //-17
    case CL_DEVICE_PARTITION_FAILED:            return "CL_DEVICE_PARTITION_FAILED";                               //-18
    case CL_KERNEL_ARG_INFO_NOT_AVAILABLE:      return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";                         //-19
    case CL_INVALID_VALUE:                      return "CL_INVALID_VALUE";
    case CL_INVALID_DEVICE_TYPE:                return "CL_INVALID_DEVICE_TYPE";
    case CL_INVALID_PLATFORM:                   return "CL_INVALID_PLATFORM";
    case CL_INVALID_DEVICE:                     return "CL_INVALID_DEVICE";
    case CL_INVALID_CONTEXT:                    return "CL_INVALID_CONTEXT";
    case CL_INVALID_QUEUE_PROPERTIES:           return "CL_INVALID_QUEUE_PROPERTIES";
    case CL_INVALID_COMMAND_QUEUE:              return "CL_INVALID_COMMAND_QUEUE";
    case CL_INVALID_HOST_PTR:                   return "CL_INVALID_HOST_PTR";
    case CL_INVALID_MEM_OBJECT:                 return "CL_INVALID_MEM_OBJECT";
    case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:    return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
    case CL_INVALID_IMAGE_SIZE:                 return "CL_INVALID_IMAGE_SIZE";
    case CL_INVALID_SAMPLER:                    return "CL_INVALID_SAMPLER";
    case CL_INVALID_BINARY:                     return "CL_INVALID_BINARY";
    case CL_INVALID_BUILD_OPTIONS:              return "CL_INVALID_BUILD_OPTIONS";
    case CL_INVALID_PROGRAM:                    return "CL_INVALID_PROGRAM";
    case CL_INVALID_PROGRAM_EXECUTABLE:         return "CL_INVALID_PROGRAM_EXECUTABLE";
    case CL_INVALID_KERNEL_NAME:                return "CL_INVALID_KERNEL_NAME";
    case CL_INVALID_KERNEL_DEFINITION:          return "CL_INVALID_KERNEL_DEFINITION";
    case CL_INVALID_KERNEL:                     return "CL_INVALID_KERNEL";
    case CL_INVALID_ARG_INDEX:                  return "CL_INVALID_ARG_INDEX";
    case CL_INVALID_ARG_VALUE:                  return "CL_INVALID_ARG_VALUE";
    case CL_INVALID_ARG_SIZE:                   return "CL_INVALID_ARG_SIZE";
    case CL_INVALID_KERNEL_ARGS:                return "CL_INVALID_KERNEL_ARGS";
    case CL_INVALID_WORK_DIMENSION:             return "CL_INVALID_WORK_DIMENSION";
    case CL_INVALID_WORK_GROUP_SIZE:            return "CL_INVALID_WORK_GROUP_SIZE";
    case CL_INVALID_WORK_ITEM_SIZE:             return "CL_INVALID_WORK_ITEM_SIZE";
    case CL_INVALID_GLOBAL_OFFSET:              return "CL_INVALID_GLOBAL_OFFSET";
    case CL_INVALID_EVENT_WAIT_LIST:            return "CL_INVALID_EVENT_WAIT_LIST";
    case CL_INVALID_EVENT:                      return "CL_INVALID_EVENT";
    case CL_INVALID_OPERATION:                  return "CL_INVALID_OPERATION";
    case CL_INVALID_GL_OBJECT:                  return "CL_INVALID_GL_OBJECT";
    case CL_INVALID_BUFFER_SIZE:                return "CL_INVALID_BUFFER_SIZE";
    case CL_INVALID_MIP_LEVEL:                  return "CL_INVALID_MIP_LEVEL";
    case CL_INVALID_GLOBAL_WORK_SIZE:           return "CL_INVALID_GLOBAL_WORK_SIZE";                           //-63
    case CL_INVALID_PROPERTY:                   return "CL_INVALID_PROPERTY";                                   //-64
    case CL_INVALID_IMAGE_DESCRIPTOR:           return "CL_INVALID_IMAGE_DESCRIPTOR";                           //-65
    case CL_INVALID_COMPILER_OPTIONS:           return "CL_INVALID_COMPILER_OPTIONS";                           //-66
    case CL_INVALID_LINKER_OPTIONS:             return "CL_INVALID_LINKER_OPTIONS";                             //-67
    case CL_INVALID_DEVICE_PARTITION_COUNT:     return "CL_INVALID_DEVICE_PARTITION_COUNT";                     //-68
   // case CL_INVALID_PIPE_SIZE:                  return "CL_INVALID_PIPE_SIZE";                                  //-69
   // case CL_INVALID_DEVICE_QUEUE:               return "CL_INVALID_DEVICE_QUEUE";                               //-70

    default:
        return "UNKNOWN ERROR CODE";
    }
}

void cl_sw_info::check_device_status(const char* log_message)
{
    if (error == CL_SUCCESS) {
            //std::cout  << log_message << " : ok" << std::endl;
    } else {
            //std::cout  << log_message << ": failed " << std::endl;
            //std::cout  << TranslateOpenCLError(error) << std::endl;
    }
}

void read_src_file(const char *file_name, char **source, size_t *src_size)
{
    FILE* fp = fopen(file_name, "rb");
    if (!fp) {
        //std::cout << "Reading file : " << file_name << " failed \n";
        exit(EXIT_FAILURE);
    }
    else {
        fseek(fp, 0, SEEK_END);
        *src_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        *source = new char[*src_size];
        if (*source == NULL) {
            //std::cout << "Not enough memory to store the file data. \n" ;
            exit(EXIT_FAILURE);
        }
        else {
            fread(*source, 1, *src_size, fp);
            //std::cout << "reading done: " << file_name << "file size :" << *src_size <<
                " Bytes  \n";
        }
    }
}
