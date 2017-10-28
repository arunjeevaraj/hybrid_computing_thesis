// Copyright (C) 2017
// Author: Arun Jeevaraj.
// All rights reserved.

// dut
#include "dut.h"
// data containers
#include "particle.h"
#include "trans_mat.h"

// data type of the reference data.
typedef double ref_data_t;


// compare the frame 1 to frame 2.
void compare_frame(ref_data_t *frame_1, ref_data_t *frame_2,
		uint64_t *mismatches, double *mse)
{
	double difference;
	*mismatches = 0;
	double sum = 0;
	for (unsigned j = 0; j < PARTICLE_CNT; j++) {
		difference = double(frame_1[j])- double(frame_2[j]);
		if (difference > THRESHOLD) {
			*mismatches += 1;
			printf("frame1 data: %f frame2 data: %f  difference %f \n",
				frame_1[j], frame_2[j], difference);
		}
		sum += pow(difference, 2);
	}
	*mse = sqrt(sum/PARTICLE_CNT);
}

void tc_to_dut_t_fr (ref_data_t *data_i, dut_frame_data_t *data_tc,
		uint64_t size)
{
	for (unsigned i = 0; i < size; i++) {
		data_tc[i] = data_i[i];
		//printf("tc:%f original:%f \n", (double)data_tc[i], data_i[i]);
	}
}

void tc_to_ref_t_fr (dut_frame_data_t *data_i, ref_data_t *data_tc,
		uint64_t size)
{
	for (unsigned i = 0; i < size ; i++) {
		data_tc[i] = (ref_data_t)data_i[i];
		//printf("tc:%f original:%f \n", data_tc[i], (double)data_i[i]);
	}
}

void tc_to_dut_t_tm (ref_data_t *data_i, dut_tmat_data_t *data_tc,
		uint64_t size)
{
	for (unsigned i = 0; i < size; i++) {
		data_tc[i] = data_i[i];
		//printf("tc:%f original:%f \n", data_tc[i], data_i[i]);
	}
}

void tc_to_ref_t_tm (dut_tmat_data_t *data_i, ref_data_t *data_tc,
		uint64_t size)
{
	for (unsigned i = 0; i < size ; i++) {
		data_tc[i] = (ref_data_t)data_i[i];
		//printf("tc:%f original:%f \n", data_tc[i], data_i[i]);
	}
}

// ********************************************************
int main()
{
	int retval = 0;
	printf("----------------------\n");
	printf("setting up stimuli for DUT\n");
	printf("----------------------\n");
	particle_s <ref_data_t> particle_in(PARTICLE_CNT);
	particle_s <ref_data_t> particle_out_sw(PARTICLE_CNT);
	particle_s <ref_data_t> particle_out_hw(PARTICLE_CNT);
	particle_s <ref_data_t> particle_out_ref(PARTICLE_CNT);

	//loading the transfer matrix.
	trans_mat <ref_data_t> tm_rdrift(0, "R_drift.bin");
	trans_mat <ref_data_t> tm_rfocus(1, "R_qffoc.bin");
	trans_mat <ref_data_t> tm_rdefocus(2, "R_qfdef.bin");
	trans_mat <ref_data_t> tm_rfocus_trans(3, (unsigned)0);

	tm_rfocus_trans.transpose_mat(tm_rfocus.getDataBuffer());

	//loading the input frame.
	particle_in.test_load_file("Beam_init.bin");
	particle_out_ref.test_load_file("Beam_end.bin");

	// print the stimuli

	//particle_in.print_particle(0);
	//printf("----------------------\n");
	//particle_out_ref.print_particle(0);
	//printf("----------------------\n");
	tm_rdrift.print_file(6, 6);
	printf("----------------------\n");
	tm_rfocus.print_file(6, 6);
	printf("----------------------\n");
	tm_rdefocus.print_file(6, 6);

	//create the expected sw result.
	printf("----------------------\n");
	printf("created the expected response\n");
	particle_out_sw.test_create_output(tm_rfocus.getDataBuffer(),
			particle_in.get_data_buffer());

	// create frames in dut_data_type for testing the results.
	dut_frame_data_t frame_i_dut [PARTICLE_CNT*FEATURE_SET];
	dut_frame_data_t frame_o_dut [PARTICLE_CNT*FEATURE_SET];
	dut_tmat_data_t tm_dut [FEATURE_SET*FEATURE_SET];

	// type cast all the frames into the data type DUT is designed to run.
	tc_to_dut_t_fr(particle_in.get_data_buffer(), frame_i_dut,
			PARTICLE_CNT*FEATURE_SET);
	tc_to_dut_t_tm(tm_rfocus_trans.getDataBuffer(), tm_dut,
			FEATURE_SET*FEATURE_SET);

	// RUN DUT
	dut (frame_i_dut, tm_dut, frame_o_dut);

	// type cast the result from the DUT to the ref data, data type.
	tc_to_ref_t_fr(frame_o_dut,  particle_out_hw.get_data_buffer(),
			PARTICLE_CNT*FEATURE_SET);
	// check results.
	printf("----------------------\n");
	printf("comparing the results.\n");
	double mse;
	uint64_t mismatches;
	printf("comparing sw model to reference data from Matlab.\n");
	compare_frame(particle_out_ref.get_data_buffer(),
			particle_out_sw.get_data_buffer(), &mismatches, &mse);
	printf("mismatches : %lu \n", mismatches);
	printf("mse        : %f \n", mse);
	printf("----------------------\n");
	if (mismatches != 0) {
		printf("Test for sw failed  !!!\n");
		retval=1;
	} else {
		printf("Test for sw passed !\n");
	}
	printf("----------------------\n");
	printf("Testing data flow.\n");
	compare_frame(particle_out_sw.get_data_buffer(),
			particle_out_hw.get_data_buffer(), &mismatches, &mse);
	printf("mismatches : %lu \n", mismatches);
	printf("mse        : %.10g \n", mse);
	if (mismatches != 0) {
  		printf("Test failed  !!!\n");
  		retval=1;
  	} else {
  		printf("Test passed !\n");
    }
	printf("----------------------\n");
	// main should return 0 for the automated testbench to find it passed, else failed.
  return mismatches;
}
