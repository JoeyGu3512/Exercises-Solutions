/*
SOURCE
Parsing arguments to the matmul test executable!!
*/

/**************************************************************/

// Include stdio
#include <stdio.h>

// Include time
#include <time.h>

// Include Lapack
#include <cblas.h>

/**************************************************************/

int matmul_ref_matmul(
    float* matA_buf, float* matB_buf, float* matC_buf, 
    int mat_i, int mat_j, int mat_k
){

    /* Old good CPU impl of matmul by blas|lapack in fortran*/
    cblas_sgemm(
        CblasRowMajor, CblasNoTrans, CblasNoTrans,
        mat_i, mat_j, mat_k, 
        1.0f,
        matA_buf, mat_k,
        matB_buf, mat_j,
        0.0f,
        matC_buf, mat_j
    );

    return 0;

}

/**************************************************************/

// end of file
