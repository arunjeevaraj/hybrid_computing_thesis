#define KERNEL_DEPTH 64
#define CHUNK_SIZE 6*KERNEL_DEPTH

// local work group size 1,1,256

typedef float data_type;
__kernel void kernelGPU1 (__global data_type *data_in,
                          __global data_type *trans_mat,
                          __global data_type *data_out)
{
    __local data_type data_buffer_i[CHUNK_SIZE];
    __local data_type data_buffer_o[CHUNK_SIZE];
    __local data_type data_buffer_trans[36];
    
    size_t  id_x  = get_global_id(2);
    size_t l_id_x = get_local_id(2);
    
    for(unsigned i= 0; i < 6 ; i++) {
        data_buffer_i[l_id_x*6 + i] = data_in[id_x*6 + i];
        data_buffer_o[l_id_x*6 + i] = 0;
    }
    for (unsigned i = 0; i < 36 ; i++) {
        data_buffer_trans[i] = trans_mat[i];
    }
    
    barrier(CLK_LOCAL_MEM_FENCE);
    
    for(unsigned k =0; k < 6 ; k++) {
        for(unsigned j =0; j < 6; j++) {
           // data_out[ id_x*CHUNK_SIZE + j + 6*i] += data_buffer_trans[j+k*6] * data_buffer_i[k + 6*i];
            data_buffer_o[j + 6*l_id_x] += data_buffer_trans[j+k*6] * data_buffer_i[k + 6*l_id_x];
        }
    }
    
    barrier(CLK_LOCAL_MEM_FENCE);
    for (unsigned i= 0; i < 6 ; i++) {
       data_out[id_x*6 + i] = data_buffer_o[l_id_x*6 + i];
    }
    
   // event_t copy_trans_mat;
    
    
    /*
    // printf("id_x %d \r\n", id_x);
    //transferring input to GMEM.
    for (unsigned i = 0; i < CHUNK_SIZE; i++) {
        data_buffer_i[i] = data_in[id_x*CHUNK_SIZE + i];
        data_buffer_o[i] = 0;
        //   if(id_x == 3)
        //     printf(" %f ", data_in[id_x*CHUNK_SIZE + i]);
        //printf("id_x %d ", id_x);
        //printf("chunk size %d ", CHUNK_SIZE);
        
        //    printf(" %f \r\n", data_buffer_i[i]);
        // data_out[id_x*CHUNK_SIZE + i] = 0;
    }
    //}
    
    //event_t event[3];
    // __global data_type* data_in_ptr = data_in+(id_x*CHUNK_SIZE);
    //event[0] = async_work_group_copy(data_buffer_i, data_in, CHUNK_SIZE, 0);
    //event[1] = async_work_group_copy(data_buffer_trans, trans_mat, 36, 0);
    //wait_group_event(1, event);
    //if(id_x==1)
    //printf(" transmat ");
    for (unsigned i = 0; i < 36 ; i++) {
        data_buffer_trans[i] = trans_mat[i];
    }
    //barrier(CLK_LOCAL_MEM_FENCE);
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
        //if(id_x ==1)
        // printf("data_out %f ", data_out[i + id_x*CHUNK_SIZE]);
        data_out[i + id_x*CHUNK_SIZE] = data_buffer_o[i];
    }
    */
}
