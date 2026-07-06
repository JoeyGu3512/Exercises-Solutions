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
#include <openblas/cblas.h>

#include <openblas/openblas_config.h>

/**************************************************************/

int matmul_ref_matmul(
    float* restrict matA_buf, 
    float* restrict matB_buf, 
    float* restrict matC_buf, 
    int mat_i, int mat_j, int mat_k,
    const float alpha,
    const float beta
){

    // openblas set num threads
    openblas_set_num_threads(openblas_get_num_threads());

    /* 
    Old good CPU impl of matmul by OpenBLAS!!!
    */
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
