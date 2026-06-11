__kernel void mmul(
   __global const float* restrict A,
   __global const float* restrict B,
   __global       float* restrict C,
   const int N,
   __local        float* restrict a_block,
   __local        float* restrict b_block,
   const int n
){
    
    // result mat coord (i,j)
    // i-Arow,j-Bcol
    int gid_i = get_global_id(0);
    int gid_j = get_global_id(1);
    int bsz_i = get_local_size(0);
    int bsz_j = get_local_size(1);
    int bid_i = get_group_id(0);
    int bid_j = get_group_id(1);
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
            lid_j*n+lid_i;
        __private int local_B_id = 
            lid_j*n+lid_i;
        __private int global_A_id =
            (bid_i*bsz_i+lid_j)*N+(n*block+lid_i);
        __private int global_B_id =
            (n*block+lid_j)*N+(bid_j*bsz_j+lid_i);

        // load to local buffer
        // if(gid_i<N && (n*block+lid_j)<N){
        //     a_block[local_A_id]=A[global_A_id];
        // }
        // if(gid_j<N && (n*block+lid_i)<N){
        //     b_block[local_B_id]=B[global_B_id];
        // }
        a_block[local_A_id] = 
            ((bid_i*bsz_i+lid_j)<N && (n*block+lid_i)<N) ?
            A[global_A_id] : 0.0f;
        b_block[local_B_id]=
            ((n*block+lid_j)<N && (bid_j*bsz_j+lid_i)<N) ?
            B[global_B_id] : 0.0f;

        // fence local mem writing
        barrier(CLK_LOCAL_MEM_FENCE);

        // calc
        if(gid_i<N && gid_j<N){
            for(int k=0;k<n;k++){
                // if(n*block+k<N){
                    dot_prod += 
                        a_block[lid_i*n+k]*b_block[k*n+lid_j];
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