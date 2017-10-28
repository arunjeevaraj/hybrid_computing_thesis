
// NDrange (1,1,Number of kernel calls.)

// data fifo depth
#define depth 8
typedef float data_type;
#define chunk_size depth*6


__kernel void __attribute__ ((reqd_work_group_size(1, 1, 1)))
//__attribute__ ((xcl_dataflow))
krnl_dm(
     __global data_type* data_current_state,
     __global data_type* data_next_state,
	 __global data_type* trans_mat
     ) {

	local data_type data_buffer_i[depth*6];
	local data_type data_buffer_o[depth*6];
	local data_type trans_mat_buffer[6*6];
	//__private data_type line_buffer[6*sizeof(data_type)];

	unsigned i;
	unsigned j,k;

	size_t id_x = get_global_id(2);
	//printf("id_x : %d \n", id_x);
	//printf("chunk_size: %d \n", chunk_size);
	__attribute__((xcl_pipeline_loop))
	Trans_mat_transfer: for(i = 0; i < 6*6; i++) {
		trans_mat_buffer[i]= trans_mat[i];
	//	printf("%f", trans_mat_buffer[i]);
	}
	__attribute__((xcl_pipeline_loop))
	Data_in_transfer: for(i = 0; i < depth*6; i++) {
		data_buffer_i[i] = data_current_state[i + chunk_size*id_x];
		data_buffer_o[i] = 0;
	//	printf("%d     %f    \n", i + chunk_size*idkrnl_vdotprod_d_x, data_current_state[i + chunk_size*id_x]);
	//	printf("%d     %f    \n", i + chunk_size*id_x, data_buffer_i[i]);
	}

	// accessing each particle data.
	__attribute__((xcl_pipeline_loop))
	PS:for(i = 0; i < depth ; i++) {
		// line buffer
		//for(k=0;k<6;k++) {
		//	line_buffer = data_buffer_i[k+6*i];
		//}
		//printf("particle id %d \n", i);
		// accessing the 6 features of  a particle
		//__attribute__((xcl_pipeline_loop))
		__attribute__((opencl_unroll_hint))
		for(k=0;k<6;k++){
			__attribute__((opencl_unroll_hint))
			for(j=0;j < 6;j++) {
				data_buffer_o[j + i*6] += trans_mat_buffer[j+k*6]*data_buffer_i[k+6*i];
			//	printf("%d,%d,%d db : %f       ",j + i*6,j+k*6,k+6*i,data_buffer_o[j+i*6]);
			}
		//	printf("\n");
		}
	}
	// storing the data back to the global memory with burst mode.
	__attribute__((xcl_pipeline_loop))
	Data_out_transfer: for(i = 0; i < depth*6 ; i++) {
		data_next_state[id_x*chunk_size + i] = data_buffer_o[i];
	}
	return;
}
