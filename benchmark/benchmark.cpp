/*******************************************************************************
// Copyright (C) 2017
// Author: Arun Jeevaraj.
// All rights reserved.
*******************************************************************************/

#include <iostream>
#include <chrono>
#include <stdint.h>
#include <cmath>

#include "particle.h"
#include "trans_mat.h"

using namespace std;

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
      			printf("frame1 data: %f frame2 data: %f  difference %f \n",
      				frame_1[j], frame_2[j], difference);
    		}
    		sum += pow(difference, 2);
  	}
  	*mse = sqrt(sum/(PARTICLE_CNT*FEATURE_SET));
}


int main()
{
    particle_s <ref_data_t> particle_frame_ping(PARTICLE_CNT);
    particle_s <ref_data_t> particle_frame_pong(PARTICLE_CNT);
    particle_s <ref_data_t> particle_frame_exp(PARTICLE_CNT);

    trans_mat <ref_data_t> tm_rdrift(0, "R_drift.bin");
    trans_mat <ref_data_t> tm_rfocus(1, "R_qffoc.bin");
    trans_mat <ref_data_t> tm_rdefocus(2, "R_qfdef.bin");

    cout << "Benchmark init" << endl;
    cout << "Reading transport matrices" << endl;
    cout << "Reading initial beam" << endl;
    particle_frame_ping.test_load_file("Beam_init.bin");// initial state.
    particle_frame_exp.test_load_file("Beam_end.bin");  // expected result.
    // start clock.
    auto start = std::chrono::high_resolution_clock::now();
    // run sim

    unsigned f_tm_sel;
    unsigned f_ping_pong;
    for (uint64_t f_i = 0; f_i < FRAME_CNT; f_i++) {
        f_tm_sel = f_i % 5;               // to select the transfer matrix.
        f_ping_pong = f_i % 2;            // to find ping or pong to be store the next frame.
          if (f_ping_pong) {// ping is output and pong is input frame.
                // choose the transfer matrix for the FODO configuration.
                if (f_tm_sel == 0 || f_tm_sel == 4) {
                  // focus
                  particle_frame_ping.test_create_output(tm_rfocus.getDataBuffer(),
                      particle_frame_pong.get_data_buffer());
                } else if (f_tm_sel == 1 || f_tm_sel == 3) {
                  //drift
                  particle_frame_ping.test_create_output(tm_rdrift.getDataBuffer(),
                      particle_frame_pong.get_data_buffer());
                } else {
                  // defocus
                  particle_frame_ping.test_create_output(tm_rdefocus.getDataBuffer(),
                      particle_frame_pong.get_data_buffer());
                }
          } else { // pong is output and ping is input frame.
                // choose the transfer matrix for the FODO configuration.
                if (f_tm_sel == 0 || f_tm_sel == 4) {
                  // focus
                  particle_frame_pong.test_create_output(tm_rfocus.getDataBuffer(),
                      particle_frame_ping.get_data_buffer());
                } else if (f_tm_sel == 1 || f_tm_sel == 3) {
                  //drift
                  particle_frame_pong.test_create_output(tm_rdrift.getDataBuffer(),
                      particle_frame_ping.get_data_buffer());
                } else {
                  // defocus
                  particle_frame_pong.test_create_output(tm_rdefocus.getDataBuffer(),
                      particle_frame_ping.get_data_buffer());
                }
          }
    }
    // get frame execution time.
    auto exe_time = std::chrono::high_resolution_clock::now();
    // end clock.
    // get total execution time.
    std::chrono::duration <double> elapsed = exe_time - start;
    cout << "Execution time for simulation : "<< elapsed.count() << "s\n";
    cout << "Execution time per frame : " << elapsed.count()/(FRAME_CNT - 1) << "s\n";

    // check if the results are the expected.
    double mse;
  	uint64_t mismatches;
    if (FRAME_CNT % 2 == 0) { // pong has the end frame.
        compare_frame(particle_frame_pong.get_data_buffer(),
      			particle_frame_exp.get_data_buffer(), &mismatches, &mse);
    } else {  // ping has the end frame.
        compare_frame(particle_frame_ping.get_data_buffer(),
            particle_frame_exp.get_data_buffer(), &mismatches, &mse);
    }
  	printf("mismatches : %lu \n", mismatches);
  	printf("mse        : %.10g \n", mse);
    if (mismatches == 0) {
        std::cout << "Expected result matches with output. Test Passed !" << '\n';
    } else {
        std::cout << "Expected result matches with output. Test Failed !" << '\n';
    }
    return 0;
}
