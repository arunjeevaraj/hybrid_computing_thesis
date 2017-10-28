#include "cl_hw_info.h"
#include <QDebug>
#include "cl_util.h"
#include <QFile>

void cl_hw_info::setup_ocl(void* data_i, void* data_tran, void* data_o,
     size_t s_data_bf, size_t s_data_tran)
{
    cl_operate = new cl_sw_info[num_devices];
    for(unsigned j = 0; j < num_devices; j++ ) {
        cl_operate[j] = cl_sw_info(0, context[j], device_id[j]);
        cl_operate[j].setDeviceType(device_type[j]);
        cl_operate[j].CreateIOEqueue();
        cl_operate[j].createMemBuff(data_i, data_o, data_tran, s_data_tran,
            s_data_bf);
    }
}


cl_hw_info::cl_hw_info()
{

}

cl_hw_info::cl_hw_info(cl_uint num_devices, cl_platform_id platform_id)
{
    device_id = new cl_device_id[num_devices];
    program = new cl_program[num_devices];
    kernel = new cl_kernel[num_devices];
    cmd_que = new cl_command_queue[num_devices];
    context = new cl_context[num_devices];
    device_type = new cl_device_type[num_devices];


    this->platform_id = platform_id;
    this->num_devices = num_devices;
}


cl_uint cl_hw_info::getNumberDevices()
{
    return this->num_devices;
}

void cl_hw_info::SetDeviceId(cl_device_id *d_id)
{
    for (unsigned i = 0; i < num_devices ; i++) {
      device_id[i] = d_id[i];
    }

}

cl_device_id cl_hw_info::getDeviceId(cl_uint d_id)
{
    return device_id[d_id];
}

void cl_hw_info::updateDeviceType()
{
    for(unsigned i = 0; i < num_devices ; i++) {
        device_type[i] = CL_DEVICE_TYPE_DEFAULT;
        clGetDeviceInfo(device_id[i], CL_DEVICE_TYPE,
                sizeof(device_type[0]), &device_type[i], 0);
    }
}

void cl_hw_info:: buildOclKernel()
{
    for (unsigned j = 0; j < num_devices; j++) {
        if (device_type[j] == CL_DEVICE_TYPE_ACCELERATOR) {
            // load the precompiled binary.
            qDebug() << "building for Fpga Device";
           // xcl_import_binary_file("TMat_mult.cl", j, "TMat_mult");
        } else if(device_type[j] == CL_DEVICE_TYPE_GPU) {
            qDebug() << "building for GPU device";
            build_program_src("../krnl/kgpu.cl","kernelGPU1", j, &error);
            check_device_status("Building kernel");
        } else if(device_type[j] == CL_DEVICE_TYPE_CPU){ // non FPGA devices..
            // build the binary for the device.
            qDebug() << "building for CPU devices";
            build_program_src("../krnl/kcpu.cl","kernelCPU1", j, &error);
           // QFile  file("./log.txt");
           // file.open(QIODevice::WriteOnly);
           // file.write("reading kernel \n");
           // file.close();
           // check_device_status("Building kernel");
        } else {
            qDebug() << "unsupported ocl Device.";
        }
    }
}

void cl_hw_info::xcl_import_binary_file(const char *xclbin_file_name, unsigned i_dev,
        const char *kernel_funct_name)
{
    int err;
    context[i_dev] = clCreateContext(NULL, 1,
            &device_id[i_dev], NULL, NULL, &err);
    check_device_status("creating context for fpga");

    //d_info[i_dev].queue[0] = clCreateCommandQueue(d_info[i_dev].context[0], d_info[i_dev].device_id,
    //		0, &d_error);
    /*
    d_info[i_dev].queue[0] =clCreateCommandQueue(d_info[i_dev].context[0], d_info[i_dev].device_id,
                               CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &d_error);
    check_device_status("creating queue for fpga");
    char *f_name_found;
    char *device_name = strdup(d_info[i_dev].device_name);
    if (device_name == NULL) {
        printf("Error: Out of Memory\n");
        exit(EXIT_FAILURE);
    }
    f_name_found = xcl_get_xclbin_name(xclbin_file_name, device_name);
    printf("INFO: Importing %s\n",f_name_found);
    if(access(f_name_found, R_OK) != 0) {
        //return NULL;

        printf("ERROR: %s xclbin not available please build\n", f_name_found);
        exit(EXIT_FAILURE);
    }
    char *krnl_bin;
    //const size_t krnl_size = load_file_to_memory(f_name_found, &krnl_bin);
    size_t krnl_size;
    read_src_file(f_name_found, &krnl_bin, &krnl_size);
    printf("INFO: Loaded file\n");
    d_info[i_dev].program[0] = clCreateProgramWithBinary(d_info[i_dev].context[0], 1,
            &d_info[i_dev].device_id, &krnl_size,
            (const unsigned char**) &krnl_bin,
            NULL, &err);
    if(err != CL_SUCCESS) {
        cout << "failed to build the kernel." << endl;
        exit(EXIT_FAILURE);
    }void cl_hw_info::
    printf("INFO: Created Binary\n");
    err = clBuildProgram(d_info[i_dev].program[0], 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS) {
        size_t len;
        char buffer[2048];

        clGetProgramBuildInfo(d_info[i_dev].program[0], d_info[i_dev].device_id,
                CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
        printf("Error: Failed to build program executable!\n");
        exit(EXIT_FAILURE);
    }

    d_info[i_dev].kernel[0] = clCreateKernel(d_info[i_dev].program[0], kernel_funct_name, &err);
        if (err != CL_SUCCESS) {
            printf("Error: Failed to create kernel for %s: %d\n", kernel_funct_name, err);
            exit(EXIT_FAILURE);
        }
    free(krnl_bin);
    */
}
// create context with device id
// create command queue with context info and device id
// read the kernel src from file.
// build the program with context info and src_file
void cl_hw_info::build_program_src(const char *file_name, const char* krnl_name,
    unsigned i_dev, cl_int *cl_error)
{
    context[i_dev] = clCreateContext(NULL, 1,
        &device_id[i_dev], NULL, NULL, &error);
    check_device_status("creating context");

    cmd_que[i_dev] = clCreateCommandQueue(context[i_dev], device_id[i_dev],
        0, &error);
    check_device_status("creating command queue");
    //cout << "-----------------------" << endl;
    //load the kernel from the source file.
    char *src_kernel;
    size_t src_size;
    // reading the kernel file.
    read_src_file(file_name, &src_kernel, &src_size);

    program[i_dev] = clCreateProgramWithSource(context[i_dev], 1,
        (const char**)&src_kernel, &src_size, &error);
    check_device_status("creating program from source.");
    error = clBuildProgram(program[i_dev], 0, NULL, NULL, NULL, NULL);
    check_device_status("building program from source.");
    kernel[i_dev] = clCreateKernel(program[i_dev], krnl_name, &error);
    check_device_status("creating kernel from program.");
    delete src_kernel;
}

void cl_hw_info::check_device_status(const char* log_message)
{
    if (error == CL_SUCCESS) {
            qDebug() << log_message << " : ok" << endl;
    } else {
            qDebug() << log_message << ": failed " << endl;
            qDebug() << TranslateOpenCLError(error) << endl;
    }
}

cl_sw_info*  cl_hw_info::getsw_info(unsigned d_id)
{
    return &cl_operate[d_id];
}

cl_kernel cl_hw_info::getKernel(unsigned d_id)
{
    return kernel[d_id];
}
