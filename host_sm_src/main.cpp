/*******************************************************************************
// Copyright (C) 2017
// Author: Arun Jeevaraj.
// All rights reserved.
*******************************************************************************/

#include "cl_sw_info.h"
#include <iostream>
#include <chrono>
#include <stdint.h>
#include <cmath>

#include "particle.h"
#include "trans_mat.h"
#include "device_state_machine.h"

typedef double ref_data_t;
const uint64_t PARTICLE_CNT = 1024*1024;
const uint64_t FRAME_CNT = 16;
const unsigned FEATURE_SET = 6;
const double THRESHOLD = 0.00001;


void compare_frame(ref_data_t *frame_1, ref_data_t *frame_2,
        uint64_t *mismatches, double *mse)
{
    double difference;
    *mismatches = 0;
    double sum = 0;
    for (unsigned j = 0; j < PARTICLE_CNT*FEATURE_SET; j++) {
            difference = double(frame_1[j])- double(frame_2[j]);
            if (difference > THRESHOLD) {
                *mismatches += 1;
                if (*mismatches < 6) {
                    printf("frame1 data: %f frame2 data: %f  difference %f \n",
                    frame_1[j], frame_2[j], difference);
                }
            }
            sum += pow(difference, 2);
    }
    *mse = sqrt(sum/(PARTICLE_CNT*FEATURE_SET));
}


int main(int argc, char *argv[])
{

    cl_int cl_error;
    cl_uint num_platforms;
    cl_uint num_devices;
    cl_error = clGetPlatformIDs(NULL, NULL, &num_platforms);
    cl_platform_id *p_list= new cl_platform_id[num_platforms];
    cl_error = clGetPlatformIDs(num_platforms, p_list, NULL);
    std::cout << "Initializing OpenCL test for CPU." << std::endl;
    char query_str[50];
    for (unsigned i = 0; i < num_platforms ; i++) {
        clGetPlatformInfo(p_list[i],CL_PLATFORM_NAME, sizeof(query_str), (void*)query_str, NULL);
        std::cout << query_str << std::endl;
    }
    std::cout << "selected device : ";
    clGetDeviceIDs(p_list[0], CL_DEVICE_TYPE_CPU, NULL, NULL, &num_devices);
    cl_device_id *d_list = new cl_device_id[num_devices];
    clGetDeviceIDs(p_list[0], CL_DEVICE_TYPE_CPU, num_devices, d_list, NULL);
    clGetDeviceInfo(d_list[0], CL_DEVICE_NAME, sizeof(query_str), (void*)query_str, NULL);
    std::cout << query_str << std::endl;

    device_config d_conf= device_config::device_single_buffer;

    // make an object of the new thread.
    device_state_machine new_device_thread(d_list[0], p_list[0], d_conf);
    new_device_thread.set_frame_count(FRAME_CNT);
    new_device_thread.start(); // starts the device state machine.

    // load the frame and transfer matrices.
    particle_s <ref_data_t> particle_frame_ping(PARTICLE_CNT);
    particle_s <ref_data_t> particle_frame_pong(PARTICLE_CNT);

    particle_s <ref_data_t> particle_initial(PARTICLE_CNT);
    particle_s <ref_data_t> particle_final_sw(PARTICLE_CNT);
    particle_s <ref_data_t> particle_frame_exp(PARTICLE_CNT);

    trans_mat <ref_data_t> tm_rdrift(0, "R_drift.bin");
    trans_mat <ref_data_t> tm_rfocus(1, "R_qffoc.bin");
    trans_mat <ref_data_t> tm_rdefocus(2, "R_qfdef.bin");
    trans_mat <ref_data_t> tm_rfocus_trans(3, (unsigned)0);

    tm_rfocus_trans.transpose_mat(tm_rfocus.getDataBuffer());

    particle_initial.test_load_file("Beam_init.bin");// initial state.
    particle_frame_exp.test_load_file("Beam_end.bin");  // expected result.
    // copy the initial frame to the frame_ping.
    particle_frame_ping.copy_data_to(particle_initial.get_data_buffer());

    // checking the inital frame and ping frame.
    //double * ptr_i = particle_initial.get_data_buffer();
    //double * ptr_o = particle_frame_ping.get_data_buffer();

    // check if the device state is initialized
    while (1) {
        if (new_device_thread.get_current_state() == device_states::st_initialized) {
            // set the frame for ocl_sw.
            cout << "copying data settings to device thread" << endl;
            new_device_thread.set_frame_data(particle_frame_ping.get_data_buffer(),
                particle_frame_pong.get_data_buffer(), particle_frame_ping.get_data_size());
            // set the current transfer matrix.
            new_device_thread.set_transfer_mat(tm_rfocus.getDataBuffer(),
                tm_rdrift.getDataBuffer(), tm_rdefocus.getDataBuffer(), tm_rfocus.getDateSize());
            // set the particle count needed for the NDRange
            new_device_thread.set_particle_cnt(PARTICLE_CNT);
            new_device_thread.state_change_command(device_states::st_copy_data_settings);
            break;
        }
    }



    std::cout << "\n Running OpenCL simulation. " << std::endl;
    // start clock.
    auto start = std::chrono::high_resolution_clock::now();

    //syncrhonization point for other threads if there is one.
    for (uint64_t f_i = 0; f_i < FRAME_CNT-1; f_i++) {
        while (1) {
            if (new_device_thread.get_current_state() == device_states::st_execute_done) {
                unsigned j = 2000;
                while(j--);
             //   std::cout << "\n after execution sync: "<< std::endl;
                break;
           }
        }
        new_device_thread.state_change_command(device_states::st_load_tm);
    }
    while (1) {
        if (new_device_thread.get_current_state() == device_states::st_execute_done) {
          //  std::cout << "\n after execution sync: "<< std::endl;
            break;
       }
    }
    // read the result.
     new_device_thread.state_change_command(device_states::st_read_device);
    // check if the device is done.
     while (1) {
         if (new_device_thread.get_current_state() == device_states::st_device_done) {
             //std::cout << "\n host: device done state machine. "<< std::endl;
             break;
        }
     }

     auto exe_time = std::chrono::high_resolution_clock::now();
     // end clock.
     // get total execution time.
     std::chrono::duration <double> elapsed = exe_time - start;
     cout << "Execution time for simulation with opencl on cpu : "<< elapsed.count() << "s\n";
     cout << "Execution time per frame with opencl on cpu : : " << elapsed.count()/(FRAME_CNT - 1) << "s\n";

     double mse;
     uint64_t mismatches;

    // particle_final_sw.test_create_output(tm_rfocus.getDataBuffer(), particle_initial.get_data_buffer());
    // particle_initial.test_create_output(tm_rdrift.getDataBuffer(), particle_final_sw.get_data_buffer());
     compare_frame(particle_frame_pong.get_data_buffer(),
          particle_frame_exp.get_data_buffer(), &mismatches, &mse);
     printf("mismatches : %lu \n", mismatches);
     printf("mse        : %.10g \n", mse);

    while(1);

    // run sim
    /*
    unsigned f_tm_sel;
    unsigned f_ping_pong;

    // the loop takes from all frame 1 to frame end.
     for (uint64_t f_i = 0; f_i < 1; f_i++) {
        f_tm_sel = f_i % 5;               // to select the transfer matrix.
        f_ping_pong = f_i % 2;            // to find ping or pong to be store the next frame.

          if (f_ping_pong) {// ping is output and pong is input frame.
                // choose the transfer matrix for the FODO configuration.
                if (f_tm_sel == 0 || f_tm_sel == 4) {
                    new_device_thread.set_current_tm(0); // Rfocus tm.
                    //check if the device is in device continue state.
                    while (1) {
                        if (new_device_thread.get_current_state() == device_states::st_execute_ready) {
                           std::cout << "\n host: device_synchronized" << std::endl;
                            break;
                       }
                    }

                    new_device_thread.state_change_command(device_states::st_execute_pong);

                   //particle_frame_ping.test_create_output(tm_rfocus.getDataBuffer(),
                   //    particle_frame_pong.get_data_buffer());
                } else if (f_tm_sel == 1 || f_tm_sel == 3) {
                  //drift
                    std::cout << "setting drift matrix as tm\n " ;
                    new_device_thread.set_current_tm(1); // Rdrift tm.
                    while (1) {
                        if (new_device_thread.get_current_state() == device_states::st_device_continue) {
                           std::cout << "\n host: device_synchronized: " << std::endl;
                            break;
                       }
                    }
                    new_device_thread.state_change_command(device_states::st_execute_pong);
                  //particle_frame_ping.test_create_output(tm_rdrift.getDataBuffer(),
                  //    particle_frame_pong.get_data_buffer());
                } else {
                  //  std::cout << "setting defocus matrix as tm\n " ;
                    new_device_thread.set_current_tm(2); // Rdefocus tm.
                    while (1) {
                        if (new_device_thread.get_current_state() == device_states::st_device_continue) {
                           std::cout << "\n host: device_synchronized: " << std::endl;
                            break;
                       }
                    }
                    new_device_thread.state_change_command(device_states::st_execute_pong);
                  // defocus
                  // particle_frame_ping.test_create_output(tm_rdefocus.getDataBuffer(),
                  //     particle_frame_pong.get_data_buffer());
                }
          } else { // pong is output and ping is input frame.
                // choose the transfer matrix for the FODO configuration.
                if (f_tm_sel == 0 || f_tm_sel == 4) {
                  // focus
                  std::cout << "setting focus matrix as tm\n " ;
                  new_device_thread.set_current_tm(0); // Rfocus tm.
                  while (1) {
                      if (new_device_thread.get_current_state() == device_states::st_device_continue) {
                         std::cout << "\n host: device_synchronized: " << std::endl;
                          break;
                     }
                  }
                  new_device_thread.state_change_command(device_states::st_execute_ping);
                } else if (f_tm_sel == 1 || f_tm_sel == 3) {
                  //drift
                 // std::cout << "setting drift matrix as tm\n " ;
                    new_device_thread.set_current_tm(1); // Rdrift tm.
                    while (1) {
                        if (new_device_thread.get_current_state() == device_states::st_device_continue) {
                           std::cout << "\n host: device_synchronized: " << std::endl;
                            break;
                       }
                    }
                    new_device_thread.state_change_command(device_states::st_execute_ping);
                } else {
                  // defocus
                 // std::cout << "setting defocus matrix as tm\n " ;
                    new_device_thread.set_current_tm(2); // Rdefocus tm.
                    while (1) {
                        if (new_device_thread.get_current_state() == device_states::st_device_continue) {
                           std::cout << "\n host: device_synchronized: " << std::endl;
                            break;
                       }
                    }
                    new_device_thread.state_change_command(device_states::st_execute_ping);
                }
          }
          // checks if the device synchronization is done.
          while (1) {
              if (new_device_thread.get_current_state() == device_states::st_execute_done) {
                  std::cout << "\n after execution sync: "<< std::endl;
                  break;
             }
          }
    }
    new_device_thread.state_change_command(device_states::st_read_device);


    while (1) {
        if (new_device_thread.get_current_state() == device_states::st_device_done) {
            std::cout << "\n device done state machine.. thread destroy. "<< std::endl;
            break;
       }
    }

    auto exe_time = std::chrono::high_resolution_clock::now();
    // end clock.
    // get total execution time.
    std::chrono::duration <double> elapsed = exe_time - start;
    cout << "Execution time for simulation with opencl on cpu : "<< elapsed.count() << "s\n";
    cout << "Execution time per frame with opencl on cpu : : " << elapsed.count()/(FRAME_CNT - 1) << "s\n";
  /*  double mse;
    uint64_t mismatches;
    while(1);
    particle_final_sw.test_create_output(tm_rfocus.getDataBuffer(), particle_initial.get_data_buffer());
    compare_frame(particle_frame_pong.get_data_buffer(),
         particle_frame_exp.get_data_buffer(), &mismatches, &mse);
    printf("mismatches : %lu \n", mismatches);
    printf("mse        : %.10g \n", mse);
    while(1);

    particle_final_sw.test_create_output(tm_rfocus.getDataBuffer(), particle_initial.get_data_buffer());
   // particle_initial.test_create_output(tm_rdrift.getDataBuffer(), particle_final_sw.get_data_buffer());


    if (FRAME_CNT % 2 == 0) { // pong has the end frame.
        compare_frame(particle_frame_pong.get_data_buffer(),
                particle_frame_exp.get_data_buffer(), &mismatches, &mse);
    } else {  // ping has the end frame.
        compare_frame(particle_frame_ping.get_data_buffer(),
                particle_frame_exp.get_data_buffer(), &mismatches, &mse);
    }
    //
    //compare_frame(particle_frame_ping.get_data_buffer(),
     //       particle_initial.get_data_buffer(), &mismatches, &mse);

    printf("mismatches : %lu \n", mismatches);
    printf("mse        : %.10g \n", mse);
    while(1);
    */
    return 0;
}
