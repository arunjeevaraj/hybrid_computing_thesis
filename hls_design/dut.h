#ifndef __DCT_H__
#define __DCT_H__
/*******************************************************************************
// Copyright (C) 2017
// Author: Arun Jeevaraj.
// All rights reserved.
*******************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <stdint.h>
#include <cmath>
// fixed point
#include <ap_int.h>

//Define word length for fixed point data
// Total word length for frame and the Integer word length.
#define W_T_FR 32
#define W_IN_FR 12
#define W_T_TM 32
#define W_IN_TM 2


//for double implementation.

//typedef double dut_frame_data_t;
//typedef double dut_tmat_data_t;

//for float implementation.

typedef float dut_frame_data_t;
typedef float dut_tmat_data_t;

//for fixed point implementation.
//typedef ap_fixed<W_T_FR, W_IN_FR> dut_frame_data_t;
//typedef ap_fixed<W_T_TM, W_IN_TM> dut_tmat_data_t;

const uint32_t PARTICLE_CNT = 256;
const double THRESHOLD = 0.00001;
#define FEATURE_SET 6

#define DCT_SIZE 8    /* defines the input matrix as 8x8 */
#define CONST_BITS  13
#define DESCALE(x,n)  (((x) + (1 << ((n)-1))) >> n)

void dut(dut_frame_data_t frame_i[PARTICLE_CNT*FEATURE_SET],
		dut_tmat_data_t trans_mat[36],
		dut_frame_data_t frame_o[PARTICLE_CNT*FEATURE_SET]);

#endif // __DCT_H__ not defined
