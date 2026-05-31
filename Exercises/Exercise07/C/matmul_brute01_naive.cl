__kernel __attribute__((work_group_size_hint(1,1,1)))
void mmul(
   __global float* A,
   __global float* B,
   __global float* C,
   const int N
){
    
    int i = get_global_id(0);
    int j = get_global_id(1);

    // OMG N3 but good experience anyway
    if(i<N && j<N){
        __private float dot_prod = 0.0;
        for(int k=0;k<N;k++){
            dot_prod += A[i*N+k] * B[k*N+j];
        }
        C[i*N+j] = dot_prod;
    }

}