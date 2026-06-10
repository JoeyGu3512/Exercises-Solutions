__kernel void mmul(
   __global float* A,
   __global float* B,
   __global float* C,
   const int N,
   __local float* a_block,
   __local float* b_block,
   const int n
){
    
    // result mat coord (i,j)
    // i-Arow,j-Bcol
    int gid_i = get_global_id(0);
    int gid_j = get_global_id(1);
    int lsz_i = get_local_size(0);
    int lsz_j = get_local_size(1);
    int lid_i = get_local_id(0);
    int lid_j = get_local_id(1);
    
    // number of blocks
    int block_num = (N+n-1)/n;

    // thread accumulate reg
    __private float dot_prod = 0.0;

    // slide over iblock
    for(int block=0;block<block_num;block++){

        // ids
        __private int local_A_id = 
            lid_i*n+lid_j;
        __private int local_B_id = 
            lid_j*n+lid_i;
        __private int global_A_id =
            (gid_i)*N+(n*block+lid_j);
        __private int global_B_id =
            (n*block+lid_i)*N+(gid_j);

        // load to local buffer
        // if(gid_i<N && (n*block+lid_j)<N){
        //     a_block[local_A_id]=A[global_A_id];
        // }
        // if(gid_j<N && (n*block+lid_i)<N){
        //     b_block[local_B_id]=B[global_B_id];
        // }
        a_block[local_A_id] = 
            (gid_i<N && (n*block+lid_j)<N) ?
            A[global_A_id] : 0.0f;
        b_block[local_B_id]=
            (gid_j<N && (n*block+lid_i)<N) ?
            B[global_B_id] : 0.0f;

        // fence local mem writing
        barrier(CLK_LOCAL_MEM_FENCE);

        // calc
        if(gid_i<N && gid_j<N){
            for(int k=0;k<n;k++){
                // if(n*block+k<N){
                    dot_prod += 
                        a_block[lid_i*n+k]*b_block[lid_j*n+k];
                // }
            }
        }

        // barrier local memory access
        barrier(CLK_LOCAL_MEM_FENCE);

    }

    // store
    if(gid_i<N && gid_j<N){
        C[gid_i*N+gid_j]=dot_prod;
    }

}