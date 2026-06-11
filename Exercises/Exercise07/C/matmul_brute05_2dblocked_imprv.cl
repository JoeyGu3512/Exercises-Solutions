

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
        ((N)*(((ss)*(n)*(K))+((ss)*(li)+(si))))                  \
        +(((ss)*(gj)+(sj)))                                    \
    ] : 0.0f;                                                  \
    }while(0)


#define FETCH_TINY_MATS_2(li,lj,gi,gj,ss,K,k,N,n,a,b,ta,tb) do{\
    ta[0][0] = a[((ss*n)*((ss)*(li)+(0)))+((ss)*( k)+(0))];    \
    ta[0][1] = a[((ss*n)*((ss)*(li)+(0)))+((ss)*( k)+(1))];    \
    ta[1][0] = a[((ss*n)*((ss)*(li)+(1)))+((ss)*( k)+(0))];    \
    ta[1][1] = a[((ss*n)*((ss)*(li)+(1)))+((ss)*( k)+(1))];    \
    tb[0][0] = b[((ss*n)*((ss)*( k)+(0)))+((ss)*(lj)+(0))];    \
    tb[0][1] = b[((ss*n)*((ss)*( k)+(0)))+((ss)*(lj)+(1))];    \
    tb[1][0] = b[((ss*n)*((ss)*( k)+(1)))+((ss)*(lj)+(0))];    \
    tb[1][1] = b[((ss*n)*((ss)*( k)+(1)))+((ss)*(lj)+(1))];    \
    }while(0)


#define TINY_MATMUL_2(ss,ta,tb,tc) do{                         \
    tc[0][0] += ta[0][0]*tb[0][0]+ta[0][1]*tb[1][0];           \
    tc[0][1] += ta[0][0]*tb[0][1]+ta[0][1]*tb[1][1];           \
    tc[1][0] += ta[1][0]*tb[0][0]+ta[1][1]*tb[1][0];           \
    tc[1][1] += ta[1][0]*tb[0][1]+ta[1][1]*tb[1][1];           \
    }while(0)



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
    int block_num = (N+(2*n)-1)/((2*n));

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
        FETCH_MEM_A(lid_i,lid_j,gid_i,gid_j,2,0,0,block,N,n,A,a_block);
        FETCH_MEM_A(lid_i,lid_j,gid_i,gid_j,2,0,1,block,N,n,A,a_block);
        FETCH_MEM_A(lid_i,lid_j,gid_i,gid_j,2,1,0,block,N,n,A,a_block);
        FETCH_MEM_A(lid_i,lid_j,gid_i,gid_j,2,1,1,block,N,n,A,a_block);
        // B to local
        FETCH_MEM_B(lid_i,lid_j,gid_i,gid_j,2,0,0,block,N,n,B,b_block);
        FETCH_MEM_B(lid_i,lid_j,gid_i,gid_j,2,0,1,block,N,n,B,b_block);
        FETCH_MEM_B(lid_i,lid_j,gid_i,gid_j,2,1,0,block,N,n,B,b_block);
        FETCH_MEM_B(lid_i,lid_j,gid_i,gid_j,2,1,1,block,N,n,B,b_block);

        // fence local mem writing
        barrier(CLK_LOCAL_MEM_FENCE);

        // calc
        if(2*gid_i<N && 2*gid_j<N){
            for(int k=0;k<n;k++){
                if(2*(n*block+k)<N){
                    FETCH_TINY_MATS_2(lid_i,lid_j,gid_i,gid_j,2,K,k,N,n,a_block,b_block,tiny_a,tiny_b);
                    TINY_MATMUL_2(2,tiny_a,tiny_b,dot_prod);
                }
            }
        }

        // barrier local memory access
        barrier(CLK_LOCAL_MEM_FENCE);

    }

    // store
    if((2*gid_i+0)<N && (2*gid_j+0)<N){
        C[(2*gid_i+0)*N+(2*gid_j+0)]=dot_prod[0][0];
    }
    if((2*gid_i+0)<N && (2*gid_j+1)<N){
        C[(2*gid_i+0)*N+(2*gid_j+1)]=dot_prod[0][1];
    }
    if((2*gid_i+1)<N && (2*gid_j+0)<N){
        C[(2*gid_i+1)*N+(2*gid_j+0)]=dot_prod[1][0];
    }
    if((2*gid_i+1)<N && (2*gid_j+1)<N){
        C[(2*gid_i+1)*N+(2*gid_j+1)]=dot_prod[1][1];
    }

}