__kernel __attribute__((work_group_size_hint(64,1,1)))
void mmul(
   __global float* A,
   __global float* B,
   __global float* C,
   const int N
){
    
    int gid_i = get_global_id(0);
    int gid_j = get_global_id(1);

    int lsz_i = get_local_size(0);
    int lid_i = get_local_id(0);
    int num_i = (N+lsz_i-1)/lsz_i;

    // A row sectional cahce
    __local float B_col_cache[2048+256];

    // thread accumulate reg
    __private float dot_prod = 0.0;

    // cpy entire arr to local
    for(int ni=0;ni<num_i;ni++){

        // load B col stride to cache
        // lazy: just one fetch per thread, maybe add a param later?
        // stride length := lsz_i
        if((ni*lsz_i+lid_i)<N && gid_j<N){    // prevent outbound
            B_col_cache[ni*lsz_i+lid_i] = B[(ni*lsz_i+lid_i)*N+gid_j];
        }

    }

    // barrier fetch to local before compute
    barrier(CLK_LOCAL_MEM_FENCE);

    // compute stride
    for(int k=0;k<N;k++){
        if(gid_i<N){
            dot_prod += A[gid_i*N+k] * B_col_cache[k];
        }
    }
    
    // write result to result buffer
    if(gid_i<N && gid_j<N){
        C[gid_i*N+gid_j] = dot_prod;
    }

}