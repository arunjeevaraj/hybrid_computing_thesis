#ifndef CL_HW_INFO_H
#define CL_HW_INFO_H

#include "ocl_h.h"
#include "cl_sw_info.h"

class cl_hw_info
{
public:
    cl_hw_info(cl_uint num_devices, cl_platform_id platform_id);
    cl_hw_info();
    void SetDeviceId(cl_device_id* d_id);
    void SetPlatformId(cl_platform_id platform_id);
    cl_uint getNumberDevices();
    cl_device_id getDeviceId(cl_uint d_id);
    void updateDeviceType();
    void buildOclKernel();
    void xcl_import_binary_file(const char *xclbin_file_name, unsigned i_dev,
        const char *kernel_funct_name);
    void check_device_status(const char* log_message);
    void build_program_src(const char *file_name, const char* krnl_name,
        unsigned i_dev, cl_int *cl_error);
    void setup_ocl(void *data_i, void *data_tran, void *data_o,
        size_t s_data_bf, size_t s_data_tran);
    cl_sw_info* getsw_info(unsigned d_id);
    cl_kernel getKernel(unsigned d_id);
private:
    cl_sw_info* cl_operate;
    bool devSelected;
    cl_device_id* device_id;
    cl_program* program;
    cl_kernel* kernel;
    cl_command_queue* cmd_que;
    cl_context* context;
    cl_platform_id platform_id;
    cl_uint num_devices;
    cl_device_type* device_type;
    cl_int error;
};

#endif // CL_HW_INFO_H
