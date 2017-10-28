#include "cl_sw_info.h"
#include "ocl_h.h"
#include <QDebug>
#include <QFile>

cl_sw_info::cl_sw_info(bool doubleBufferEnable, cl_context context, cl_device_id device_id)
{
    qDebug() << "context allocation done # 1";
    doubleBuffer = doubleBufferEnable;
    memBuffer_i = new cl_mem;
    memBuffer_o = new cl_mem;
    memBuffer_trans = new cl_mem;
    this->context = context;
    this->device_id = device_id;
}

cl_sw_info :: cl_sw_info ()
{


}

void cl_sw_info::createOOEqueue()
{
    cmd_queue = clCreateCommandQueue(context, device_id,
          CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | CL_QUEUE_PROFILING_ENABLE, &error);
    if (error != CL_SUCCESS) {
        qDebug() << "cmd_queue failed to create" ;
    }
    queue_config = 0;
}

void cl_sw_info::createMemBuff( void* data_buffer_i, void* data_buffer_o, void* data_trans,
    size_t size_trans, size_t size_data_buffer)
{
    qDebug() << "Mapping data in device level";
    this->data_buffer_i = data_buffer_i;
    this->data_buffer_o = data_buffer_o;
    this->data_trans = data_trans;
    this->size_trans = size_trans;
    this->size_data_buffer = size_data_buffer;
    //qDebug() << "size of transfer matrix" << size_trans;
    qDebug() << "size of data buffer" << size_data_buffer;
    //float* float_ptr = (float*) data_trans;
    if (queue_config == 0) { // ooe queue
        memBuffer_i[0] = clCreateBuffer(context, CL_MEM_READ_ONLY| CL_MEM_USE_HOST_PTR,
             size_data_buffer, data_buffer_i, NULL);
        memBuffer_trans[0] = clCreateBuffer(context, CL_MEM_READ_ONLY| CL_MEM_USE_HOST_PTR,
             size_trans, data_trans, NULL);
        memBuffer_o[0] = clCreateBuffer(context, CL_MEM_WRITE_ONLY, size_data_buffer, NULL, NULL);
    } else { // ioe queue.
        qDebug() << "----------------------";
        memBuffer_i[0] = clCreateBuffer(context, CL_MEM_READ_WRITE| CL_MEM_USE_HOST_PTR,
             size_data_buffer, data_buffer_i, NULL);
        memBuffer_trans[0] = clCreateBuffer(context, CL_MEM_READ_ONLY| CL_MEM_USE_HOST_PTR,
             size_trans, data_trans, NULL);


       // memBuffer_i[0]= clCreateBuffer(context, CL_MEM_READ_ONLY, size_data_buffer, NULL, NULL);
        //clEnqueueWriteBuffer(cmd_queue, memBuffer_i[0], CL_FALSE, 0, size_data_buffer,
          //  data_buffer_i, 0, NULL, NULL);
       // memBuffer_trans[0] = clCreateBuffer(context, CL_MEM_READ_ONLY, size_trans, NULL, NULL);
        //clEnqueueWriteBuffer(cmd_queue, memBuffer_trans[0], CL_FALSE, 0, size_data_buffer,
          //  data_buffer_o, 0, NULL, NULL);
        memBuffer_o[0] = clCreateBuffer(context, CL_MEM_READ_WRITE, size_data_buffer, NULL, NULL);
    }
}

void cl_sw_info::writeToDevice()
{
    if (queue_config == 0) { //ooe queue
        clEnqueueMigrateMemObjects(cmd_queue, 1, memBuffer_i, 0, 0, NULL, &WriteEvent[0] );
        clFinish(cmd_queue);
        clEnqueueMigrateMemObjects(cmd_queue, 1, memBuffer_trans, 0, 0, NULL, &WriteEvent[1] );
        clFinish(cmd_queue);
    } else { //inorder queue.
        qDebug() << "writing through inorder queue.";
        clEnqueueMigrateMemObjects(cmd_queue, 1, memBuffer_i, 0, 0, NULL, &WriteEvent[0] );
        clFinish(cmd_queue);
        clEnqueueMigrateMemObjects(cmd_queue, 1, memBuffer_trans, 0, 0, NULL, &WriteEvent[1] );
        clFinish(cmd_queue);

       //error = clEnqueueWriteBuffer(cmd_queue, memBuffer_i[0], CL_FALSE, 0, size_data_buffer,
         //   data_buffer_i, 0, NULL, &WriteEvent[0]);
       //if(error != CL_SUCCESS) {
         //   qDebug() << "Enqueue failed.";
       //}
        //clFinish(cmd_queue);
      // error = clEnqueueWriteBuffer(cmd_queue, memBuffer_trans[0], CL_FALSE, 0, size_trans,
         //   this->data_trans, 0, NULL, &WriteEvent[1]);
       //if(error != CL_SUCCESS) {
         //   qDebug() << "Enqueue failed.";
       //}
       // clFinish(cmd_queue);
    }
}

void cl_sw_info::CreateIOEqueue()
{
    qDebug() << "creating inorder command queue # 2";
    cmd_queue = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE,
    &error);
    queue_config = 1;
    if(error != CL_SUCCESS) {
        qDebug() << "The in-order command queue failed to create.";
    }
}

void cl_sw_info::kernelExecute(cl_kernel kernel,ulong particle_cnt)
{

    this->kernel= kernel;
    this->particle_cnt = particle_cnt;
    error = clSetKernelArg(kernel, 0, sizeof(cl_mem), memBuffer_i);
    if(error != CL_SUCCESS) {
        qDebug() << "kernel linking arg 0 failed.";
    }
    error = clSetKernelArg(kernel, 1, sizeof(cl_mem), memBuffer_trans);
    if(error != CL_SUCCESS) {
        qDebug() << "kernel linking arg 1 failed.";
    }
    error = clSetKernelArg(kernel, 2, sizeof(cl_mem), memBuffer_o);
    if(error != CL_SUCCESS) {
        qDebug() << "kernel linking arg 2 failed.";
    }

    if (device_type == CL_DEVICE_TYPE_CPU) {
        size_t globalSize[] = {1, 1, particle_cnt/16};
        error = clEnqueueNDRangeKernel(cmd_queue, kernel, 3, NULL, globalSize, 0, 0, NULL, &exeEvent );
        //size_t globalSize[] = {1, 1, particle_cnt};
        //size_t localSize[] ={1, 1, 4};
       // error = clEnqueueNDRangeKernel(cmd_queue, kernel, 3, NULL, globalSize, localSize,
         //   0, NULL, &exeEvent );
    } else if(device_type == CL_DEVICE_TYPE_GPU) {
        size_t globalSize[] = {1, 1, particle_cnt};
        size_t localSize[] ={1, 1, 64};
        error = clEnqueueNDRangeKernel(cmd_queue, kernel, 3, NULL, globalSize, localSize,
            0, NULL, &exeEvent );
    } else {
        qDebug() << "fpga here";
    }
    if(error != CL_SUCCESS) {
        qDebug() << "kernel execution failed.";
    }
}

void cl_sw_info:: setDeviceType(cl_device_type d_type)
{
    this->device_type = d_type;
}

ulong cl_sw_info:: getEventDuration(cl_event event)
{
    ulong start, stop;
    error = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(ulong), &start, NULL);
    if(error != CL_SUCCESS){
        qDebug() << "even profiling failed";
    }
    error = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(ulong), &stop, NULL);
    if(error != CL_SUCCESS){
        qDebug() << "even profiling failed";
    }
    return stop - start;
}

void cl_sw_info :: kernelRunsim(ulong count)
{
    qDebug() <<  "iteration count inside KernelRunsim" << count;
    if (count % 2 == 0) {
        qDebug() << "going for ping pong";
        error = clSetKernelArg(kernel, 0, sizeof(cl_mem), memBuffer_o);
        if(error != CL_SUCCESS) {
            qDebug() << "kernel linking arg 0 failed.";
        }
        error = clSetKernelArg(kernel, 2, sizeof(cl_mem), memBuffer_i);
        if(error != CL_SUCCESS) {
            qDebug() << "kernel linking arg 2 failed.";
        }
    } else {
        error = clSetKernelArg(kernel, 0, sizeof(cl_mem), memBuffer_i);
        if(error != CL_SUCCESS) {
            qDebug() << "kernel linking arg 0 failed.";
        }
        error = clSetKernelArg(kernel, 2, sizeof(cl_mem), memBuffer_o);
        if(error != CL_SUCCESS) {
            qDebug() << "kernel linking arg 2 failed.";
        }
    }

    error = clSetKernelArg(kernel, 1, sizeof(cl_mem), memBuffer_trans);
    if(error != CL_SUCCESS) {
        qDebug() << "kernel linking arg 1 failed.";
    }


    if (device_type == CL_DEVICE_TYPE_CPU) {
        size_t globalSize[] = {1, 1, particle_cnt/16};
        error = clEnqueueNDRangeKernel(cmd_queue, kernel, 3, NULL, globalSize, 0, 0, NULL, &exeEvent );
        //size_t globalSize[] = {1, 1, particle_cnt};
        //size_t localSize[] ={1, 1, 4};
       // error = clEnqueueNDRangeKernel(cmd_queue, kernel, 3, NULL, globalSize, localSize,
         //   0, NULL, &exeEvent );
    } else if(device_type == CL_DEVICE_TYPE_GPU) {
        size_t globalSize[] = {1, 1, particle_cnt};
        size_t localSize[] ={1, 1, 32};
        error = clEnqueueNDRangeKernel(cmd_queue, kernel, 3, NULL, globalSize, localSize,
            0, NULL, &exeEvent );
    } else {
        qDebug() << "fpga here";
    }
    if(error != CL_SUCCESS) {
        qDebug() << "kernel execution failed.";
    }
    // wait for the execution to be done.
    clFinish(cmd_queue);
}

void cl_sw_info :: readFromDevice(ulong count)
{
    clFinish(cmd_queue);    // wait for execution to be done.
    if(count % 2 == 0) {
        clEnqueueReadBuffer(cmd_queue, memBuffer_o[0], CL_TRUE, 0, size_data_buffer, data_buffer_o,
            0 , NULL, &ReadEvent);
    } else {
        qDebug() << "reading mem buffer in as output";
        clEnqueueReadBuffer(cmd_queue, memBuffer_i[0], CL_TRUE, 0, size_data_buffer, data_buffer_o,
            0 , NULL, &ReadEvent);
    }
    clFinish(cmd_queue);    // wait for reading the data back.
    ulong ExecDuration = getEventDuration(exeEvent);
    ulong ReadDuration = getEventDuration(ReadEvent);
    ulong WriteDuration = getEventDuration(WriteEvent[0]);
    ulong WriteDurationTran = getEventDuration(WriteEvent[0]);

    qDebug() << "Execution time : " << ExecDuration;
    qDebug() << "Time to read :" << ReadDuration;
    qDebug() << "Time to write data :" << WriteDuration;
    qDebug() << "Time to write trans data :" << WriteDurationTran;

    QString toFile = QString("Exe %1 TR %2 TW %3\
         TWT %4 \r\n").arg(ExecDuration).arg(ReadDuration).arg(WriteDuration).arg(WriteDurationTran);
    QFile file("./log.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QByteArray ba = toFile.toLatin1();
    const char *c_str2 = ba.data();
    file.write("performance Analysis. \n");
    file.write(c_str2);
    file.close();
}
