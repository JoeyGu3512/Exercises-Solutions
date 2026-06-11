
#define FETCH_MEM_A(li,lj,gi,gj,ss,si,sj,K,N,n,A,a) do{        \
    a[                                                         \
        ((ss*n)*((ss)*(li)+(si)))                              \
        +((ss)*(lj)+(sj))                                      \
    ] =                                                        \
    (                                                          \
        (((ss)*(gi)+(si)))                < (N) &&             \
        (((ss)*(n)*(K))+((ss)*(lj)+(sj))) < (N)                \
    ) ?                                                        \
    A[                                                         \
        ((N)*((ss)*(gi)+(si)))                                 \
        +(((ss)*(n)*(K))+((ss)*(lj)+(sj)))                     \
    ] : 0.0f;                                                  \
    }while(0)


#define FETCH_MEM_B(li,lj,gi,gj,ss,si,sj,K,N,n,B,b) do{        \
    b[                                                         \
        ((ss*n)*((ss)*(li)+(si)))                              \
        +((ss)*(lj)+(sj))                                      \
    ] =                                                        \
    (                                                          \
        (((ss)*(gj)+(sj)))                < (N) &&             \
        (((ss)*(n)*(K))+((ss)*(li)+(si))) < (N)                \
    ) ?                                                        \
    B[                                                         \
        ((N)*(((ss)*(n)*(K))+((ss)*(li)+(si))))                \
        +(((ss)*(gj)+(sj)))                                    \
    ] : 0.0f;                                                  \
    }while(0)

#define FETCH_BLOCK_MATS(li,lj,gi,gj,ss,K,N,n,A,B,a,b) do{     \
    __attribute__((opencl_unroll_hint)) for(int i=0;i<ss;i++){ \
    __attribute__((opencl_unroll_hint)) for(int j=0;j<ss;j++){ \
        FETCH_MEM_A(li,lj,gi,gj,ss,i,j,K,N,n,A,a);             \
    }}                                                         \
    __attribute__((opencl_unroll_hint)) for(int i=0;i<ss;i++){ \
    __attribute__((opencl_unroll_hint)) for(int j=0;j<ss;j++){ \
        FETCH_MEM_B(li,lj,gi,gj,ss,i,j,K,N,n,B,b);             \
    }}                                                         \
    }while(0)

#define FETCH_TINY_MATS(li,lj,gi,gj,ss,K,k,N,n,a,b,ta,tb) do{  \
    __attribute__((opencl_unroll_hint)) for(int i=0;i<ss;i++){ \
    __attribute__((opencl_unroll_hint)) for(int j=0;j<ss;j++){ \
        ta[i][j] = a[((ss*n)*((ss)*(li)+(i)))+((ss)*( k)+(j))];\
    }}                                                         \
    __attribute__((opencl_unroll_hint)) for(int i=0;i<ss;i++){ \
    __attribute__((opencl_unroll_hint)) for(int j=0;j<ss;j++){ \
        tb[i][j] = b[((ss*n)*((ss)*( k)+(i)))+((ss)*(lj)+(j))];\
    }}                                                         \
    }while(0)

#define TINY_MATMUL(ss,ta,tb,tc) do{                           \
    __attribute__((opencl_unroll_hint)) for(int i=0;i<ss;i++){ \
    __attribute__((opencl_unroll_hint)) for(int j=0;j<ss;j++){ \
    __attribute__((opencl_unroll_hint)) for(int k=0;k<ss;k++){ \
            tc[i][j] += ta[i][k]*tb[k][j];                     \
    }}}                                                        \
    }while(0)

#define STORE_RESULT(ss,tc,C) do{                              \
    __attribute__((opencl_unroll_hint)) for(int i=0;i<ss;i++){ \
    __attribute__((opencl_unroll_hint)) for(int j=0;j<ss;j++){ \
        if((ss*gid_i+i)<N && (ss*gid_j+j)<N){                  \
            C[(ss*gid_i+i)*N+(ss*gid_j+j)]=dot_prod[i][j];     \
        };                                                     \
    }}                                                         \
    }while(0)

#define SS 4

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
    int block_num = (N+(SS*n)-1)/((SS*n));

    // thread accumulate reg
    __private float tiny_a[SS][SS];
    __private float tiny_b[SS][SS];
    __private float dot_prod[SS][SS] = {
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f
    };

    // slide over iblock
    for(int block=0;block<block_num;block++){

        // load to local buffer
        FETCH_BLOCK_MATS(lid_i,lid_j,gid_i,gid_j,SS,block,N,n,A,B,a_block,b_block);

        // fence local mem writing
        barrier(CLK_LOCAL_MEM_FENCE);

        // calc
        if(SS*gid_i<N && SS*gid_j<N){
            for(int k=0;k<n;k++){
                if(SS*(n*block+k)<N){
                    FETCH_TINY_MATS(lid_i,lid_j,gid_i,gid_j,SS,K,k,N,n,a_block,b_block,tiny_a,tiny_b);
                    TINY_MATMUL(SS,tiny_a,tiny_b,dot_prod);
                }
            }
        }

        // barrier local memory access
        barrier(CLK_LOCAL_MEM_FENCE);

    }

    // store
    STORE_RESULT(SS,dot_prod,C);

}