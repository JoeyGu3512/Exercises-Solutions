

#define FETCH_MEM_A(li,lj,gi,gj,ss,si,sj,K,N,n,A,a)            \
    /* beg of macro */{                                        \
    if(                                                        \
        (((ss)*(gi)+(si)))                < (N) &&             \
        (((ss)*(n)*(K))+((ss)*(lj)+(sj))) < (N)                \
    )                                                          \
    {                                                          \
        a[                                                     \
            ((n)*((ss)*(li)+(si)))                             \
            +((ss)*(lj)+(sj))                                  \
        ] =                                                    \
        A[                                                     \
            ((N)*((ss)*(gi)+(si)))                             \
            +(((ss)*(n)*(K))+((ss)*(lj)+(sj)))                 \
        ];                                                     \
    }                                                          \
    }/* end of macro */


#define FETCH_MEM_B(li,lj,gi,gj,ss,si,sj,K,N,n,B,b)            \
    /* beg of macro */{                                        \
    if(                                                        \
        (((ss)*(gj)+(sj)))                < (N) &&             \
        (((ss)*(n)*(K))+((ss)*(li)+(si))) < (N)                \
    )                                                          \
    {                                                          \
        b[                                                     \
            ((n)*((ss)*(lj)+(sj)))                             \
            +((ss)*(li)+(si))                                  \
        ] =                                                    \
        B[                                                     \
            ((N)*(((ss)*(n)*(K))+((ss)*(li)+(si))))            \
            +(((ss)*(gj)+(sj)))                                \
        ];                                                     \
    }                                                          \
    }/* end of macro */


#define FETCH_TINY_MATS(li,lj,gi,gj,ss,K,k,N,n,a,b,ta,tb)  \
    /* beg of macro */{                                        \
      \
    }/* end of macro */



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
    int block_num = (N+2*n-1)/(2*n);

    // thread accumulate reg
    __private float tiny_a[2][2] = {
        0.0f, 0.0f,
        0.0f, 0.0f
    };
    __private float tiny_b[2][2] = {
        0.0f, 0.0f,
        0.0f, 0.0f
    };
    __private float dot_prod[2][2] = {
        0.0f, 0.0f,
        0.0f, 0.0f
    };

    // slide over iblock
    for(int block=0;block<block_num;block++){

        // load to local buffer
        // A to local
        FETCH_MEM_A(
            lid_i,lid_j,gid_i,gid_j,
            2,0,0,block,
            N,n,A,a_block
        );
        FETCH_MEM_A(
            lid_i,lid_j,gid_i,gid_j,
            2,0,1,block,
            N,n,A,a_block
        );
        FETCH_MEM_A(
            lid_i,lid_j,gid_i,gid_j,
            2,1,0,block,
            N,n,A,a_block
        );
        FETCH_MEM_A(
            lid_i,lid_j,gid_i,gid_j,
            2,1,1,block,
            N,n,A,a_block
        );
        // B to local
        FETCH_MEM_B(
            lid_i,lid_j,gid_i,gid_j,
            2,0,0,block,
            N,n,B,b_block
        );
        FETCH_MEM_B(
            lid_i,lid_j,gid_i,gid_j,
            2,1,0,block,
            N,n,B,b_block
        );
        FETCH_MEM_B(
            lid_i,lid_j,gid_i,gid_j,
            2,0,1,block,
            N,n,B,b_block
        );
        FETCH_MEM_B(
            lid_i,lid_j,gid_i,gid_j,
            2,1,1,block,
            N,n,B,b_block
        );

        // fence local mem writing
        barrier(CLK_LOCAL_MEM_FENCE);

        // calc
        if(gid_i<N && gid_j<N){
            for(int k=0;k<n;k++){
                if(n*block+k<N){
                    dot_prod += 
                        a_block[lid_i*n+k]*b_block[lid_j*n+k];
                }
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