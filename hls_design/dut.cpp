/*******************************************************************************
// Copyright (C) 2017
// Author: Arun Jeevaraj.
// All rights reserved.
*******************************************************************************/
#include "dut.h"

void frame_mul( dut_frame_data_t buffer_in_2d[PARTICLE_CNT][FEATURE_SET],
		dut_tmat_data_t buffer_tm[FEATURE_SET][FEATURE_SET],
		dut_frame_data_t buffer_out_2d[PARTICLE_CNT][FEATURE_SET])
{

	dut_frame_data_t buffer_res[FEATURE_SET][FEATURE_SET];
	dut_frame_data_t buffer_acc[FEATURE_SET];
	unsigned p_i, f_i, i_i;
CLR_ACC_loop:
	for (f_i = 0; f_i < FEATURE_SET; f_i++) {
		buffer_acc[f_i] = 0;
	}



PCNT_Loop_mul:
	for (p_i = 0; p_i < PARTICLE_CNT; p_i++) {
FSET_Loop_mul:
		for (f_i = 0; f_i < FEATURE_SET; f_i++) {
INN_Loop_mul:
			for (i_i = 0; i_i < FEATURE_SET; i_i++) {
				buffer_res[i_i][f_i] = buffer_tm[i_i][f_i]*buffer_in_2d[p_i][f_i];
			}
		}

FSET_Loop_2_mul:
		for (f_i = 0; f_i < FEATURE_SET; f_i++) {
SUM_ACCUM_mul:
			for (i_i = 0; i_i < FEATURE_SET; i_i++) {
				buffer_acc[f_i] += buffer_res[f_i][i_i];
			}
		}

FSET_Loop_3_mul:
		for (f_i = 0; f_i < FEATURE_SET; f_i++) {
			buffer_out_2d[p_i][f_i] = buffer_acc[f_i];
			buffer_acc[f_i] = 0;
		}

	}
}

// reads the data to the internal buffer.
// clears the output buffer to zero.
void read_data(dut_frame_data_t input[FEATURE_SET*PARTICLE_CNT],
		dut_frame_data_t buf[PARTICLE_CNT][FEATURE_SET])
{
   unsigned p_i, f_i;

PCNT_Loop_read:
   for (p_i = 0; p_i < PARTICLE_CNT; p_i++) {
Feat_Loop_read:
      for (f_i = 0; f_i < FEATURE_SET; f_i++) {
         buf[p_i][f_i] = input[p_i*FEATURE_SET + f_i];
      }
   }
}
// reads the transfer matrix to the internal buffer.
void read_tm(dut_tmat_data_t input[FEATURE_SET*FEATURE_SET],
		dut_tmat_data_t buf[FEATURE_SET][FEATURE_SET])
{
	unsigned r_i, c_i;
Row_Loop_read:
	for (r_i = 0; r_i < FEATURE_SET; r_i++) {
Col_Loop_read:
		for (c_i = 0; c_i < FEATURE_SET; c_i++) {
			buf[r_i][c_i] = input[r_i*FEATURE_SET + c_i];
		}
	}
}
// writes the result to the output fifo.
void write_data(dut_frame_data_t buf[PARTICLE_CNT][FEATURE_SET],
		dut_frame_data_t output[FEATURE_SET*PARTICLE_CNT])
{
	unsigned p_i, f_i;

PCNT_Loop_write:
   for (p_i = 0; p_i < PARTICLE_CNT; p_i++) {
Feat_Loop_write:
      for (f_i = 0; f_i < FEATURE_SET; f_i++)
         output[p_i*FEATURE_SET + f_i] = buf[p_i][f_i];
   }
}

// target HLS function to synthesize HDL.
void dut (dut_frame_data_t *frame_i,//[PARTICLE_CNT*FEATURE_SET],
		dut_tmat_data_t *trans_mat,//[36],
		dut_frame_data_t *frame_o)//[PARTICLE_CNT*FEATURE_SET])
{
	dut_frame_data_t frame_2d_in[PARTICLE_CNT][FEATURE_SET];
	dut_frame_data_t frame_2d_out[PARTICLE_CNT][FEATURE_SET];
	dut_tmat_data_t buff_trans_mat[FEATURE_SET][FEATURE_SET];

	read_data(frame_i, frame_2d_in);
	read_tm(trans_mat, buff_trans_mat);
	frame_mul(frame_2d_in, buff_trans_mat, frame_2d_out);
	write_data(frame_2d_out, frame_o);

  return;
}
