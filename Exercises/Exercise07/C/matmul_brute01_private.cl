__kernel void mmul(
   __global float* A,
   __global float* B,
   __global float* C,
   const int N
){
    
    int i = get_global_id(0);

    // wtf???
    __private float a_row[2048];
    if(i<N){
        for(int k=0;k<N;k++){
            a_row[k] = A[i*N+k];
        }
    }

    // OMG N3 but good experience anyway
    if(i<N){
        __private float dot_prod;
        for (int j = 0; j < N; j++) {
            dot_prod = 0.0;
            for(int k=0;k<N;k++){
                dot_prod += a_row[k] * B[k*N+j];
            }
            C[i*N+j] = dot_prod;
        }
    }

}