#pragma OPENCL EXTENSION cl_khr_fp64 : enable
typedef float data_type;
__kernel void Mat_mul(__constant data_type *R_drift, __constant  data_type *R_qf, __constant data_type *R_qdf,
	__global data_type *data_out, __global data_type *data_in,  int cell_i,  int cell_offset, int old_cell_offset)
{
    int gid = get_global_id(0);
    data_type loc_data[5*6];
    int i,j;
 //   printf("cell i : %d \r\n", cell_i);
 //   printf("gid : %f \r\n", gid);
    // focus
    if (cell_i == 0) {
        for ( i = 0; i < 6; i++) {
            loc_data[i] = 0;
            for (j = 0; j < 6 ; j++ ) {
                loc_data[i] += R_qf[6*i + j]* data_in[j + 6*gid];
            }
        }
    } else { //  access from data_out buffer.
        for ( i = 0; i < 6; i++) {
            loc_data[i] = 0;
            for (j = 0; j < 6 ; j++ ) {
                loc_data[i] += R_qf[6*i + j]*data_out[j + 30*gid + old_cell_offset];
            }
        }
    }
    // drift
    for ( i = 0; i < 6; i++) {
        loc_data[6+i] = 0;
        for (j = 0; j < 6 ; j++ ) {
            loc_data[6+i] += R_drift[6*i + j]*loc_data[j];
        }
    }
    // defocus
    for ( i = 0; i < 6; i++) {
        loc_data[12+i] = 0;
        for (j = 0; j < 6 ; j++ ) {
            loc_data[12+i] += R_qdf[6*i + j]*loc_data[6 + j];
        }
    }
    // drift
    for ( i = 0; i < 6; i++) {
        loc_data[18 + i] = 0;
        for (j = 0; j < 6 ; j++ ) {
            loc_data[18 + i] += R_drift[6*i + j]*loc_data[12 + j];
        }
    }
    // focus.
    for ( i = 0; i < 6; i++) {
        loc_data[24 + i] = 0;
        for (j = 0; j < 6 ; j++ ) {
            loc_data[24 + i] += R_qf[6*i + j]*loc_data[18 + j];
        }
    }
    
    // store the local memory back to the global memory.
    for (i = 0; i < 30 ; i++) {
        data_out[gid*30 + i + cell_offset] = loc_data[i];
    }  
}

