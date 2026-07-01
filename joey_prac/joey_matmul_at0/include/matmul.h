#ifndef MATMUL_H
#define MATMUL_H
/*
HEADER
Universal Definitions for Matmul Testing
*/

/**************************************************************/

/*
The struct holding matrix buffers and the sizes for testing
*/
struct matmul_testing_t {

    /* Matrix sizes */
    // Matrix: A-row AB-row
    int mat_i;
    // Matrix: B-col AB-col
    int mat_j;
    // Matrix: A-col B-row
    int mat_k;

    /* Host Buffers */
    // Matrix A - (i,k)
    float* matA;
    // Matrix B - (k,j)
    float* matB;
    // Matrix AB - (i,j) - result
    float* matAB_res;
    // Matrix AB - (i,j) - reference
    float* matAB_ref;

};

/**************************************************************/

// End of file
#endif