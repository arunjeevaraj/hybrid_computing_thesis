#define KERNEL_DEPTH 16
#define CHUNK_SIZE 6*KERNEL_DEPTH
typedef float data_type;
__kernel void kernelCPU1 (__global data_type *data_in,
                         __global data_type *trans_mat,
                         __global data_type *data_out)
{
    __local data_type data_buffer_i[CHUNK_SIZE];
    __local data_type data_buffer_o[CHUNK_SIZE];
    __local data_type data_buffer_trans[36];
    size_t  id_x = get_global_id(2);
  
    //transferring input to GMEM.
    for (unsigned i = 0; i < CHUNK_SIZE; i++) {
        data_buffer_i[i] = data_in[id_x*CHUNK_SIZE + i];
        data_buffer_o[i] = 0;
    }
     for (unsigned i = 0; i < 36 ; i++) {
        data_buffer_trans[i] = trans_mat[i];
    }
        
    //do the computation.
    for(unsigned i =0; i< KERNEL_DEPTH; i++) {
        for(unsigned k =0; k < 6 ; k++) {
            for(unsigned j =0; j < 6; j++) {
                //data_out[ id_x*CHUNK_SIZE + j + 6*i] += data_buffer_trans[j+k*6] * data_buffer_i[k + 6*i];
                data_buffer_o[j + 6*i] += data_buffer_trans[j+k*6] * data_buffer_i[k + 6*i];
                //if(id_x == 1)
                  //  printf("fe:%d %f ",i, data_buffer_o[i]);
            }
        }
    }
    
    
    // transferring output back to GMEM
    for(unsigned i = 0; i < CHUNK_SIZE ; i++) {
        data_out[i + id_x*CHUNK_SIZE] = data_buffer_o[i];
    }
    
}
