/*
SOURCE
Parsing arguments to the matmul test executable!!
*/

/**************************************************************/

// Include its own header
#include "matmul_pre.h"

// Include stdio
#include <stdio.h>

// Include stdlib
#include <stdlib.h>

// Define OpenMP shedule slide size
#define OMP_SCHEDULE_SIZE 128

// Debug toggle
#if 1
#ifndef DEBUG_MATMUL_PREPARE
#define DEBUG_MATMUL_PREPARE
#endif
#endif

/**************************************************************/
/******* MATMUL I - Allocation ********************************/
/**************************************************************/

// Allocation
int matmul_alloc(
    struct matmul_testing_t *matmul_test,
    int mat_i, int mat_j, int mat_k
){

    // check
    if(matmul_test==NULL){
        return 1;
    }

    // save sizes
    matmul_test->mat_i = mat_i;
    matmul_test->mat_j = mat_j;
    matmul_test->mat_k = mat_k;

    // calculate sizes
    int matA_size = mat_i*mat_k;
    int matB_size = mat_j*mat_k;
    int matAB_size = mat_i*mat_j;


    // alloc (and check)
    float *host_buffer_A;
    float *host_buffer_B;
    float *host_buffer_ABres;
    float *host_buffer_ABref;

    host_buffer_A = malloc(matA_size*sizeof(float));
    if(host_buffer_A==NULL){
        printf(
            "[ERROR] Failed to allocate " \
            "%d Bytes for {host_buffer_A}! \n",
            matA_size*sizeof(float)
        );
        return 2;
    }

    host_buffer_B = malloc(matB_size*sizeof(float));
    if(host_buffer_B==NULL){
        printf(
            "[ERROR] Failed to allocate " \
            "%d Bytes for {host_buffer_B}! \n",
            matB_size*sizeof(float)
        );
        free(host_buffer_A); host_buffer_A=NULL;
        return 2;
    }
    
    host_buffer_ABres = malloc(matAB_size*sizeof(float));
    if(host_buffer_ABres==NULL){
        printf(
            "[ERROR] Failed to allocate " \
            "%d Bytes for {host_buffer_ABresult}! \n",
            matAB_size*sizeof(float)
        );
        free(host_buffer_A); host_buffer_A=NULL;
        free(host_buffer_B); host_buffer_B=NULL;
        return 2;
    }
    
    host_buffer_ABref = malloc(matAB_size*sizeof(float));
    if(host_buffer_ABref==NULL){
        printf(
            "[ERROR] Failed to allocate " \
            "%d Bytes for {host_buffer_ABreference}! \n",
            matAB_size*sizeof(float)
        );
        free(host_buffer_A); host_buffer_A=NULL;
        free(host_buffer_B); host_buffer_B=NULL;
        free(host_buffer_ABres); host_buffer_ABres=NULL;
        return 2;
    }

    // Give the buffers to the struct
    matmul_test->matA = host_buffer_A;
    matmul_test->matB = host_buffer_B;
    matmul_test->matAB_res = host_buffer_ABres;
    matmul_test->matAB_ref = host_buffer_ABref;

    // return 0 for success
    return 0;

}

// Free
int matmul_free(
    struct matmul_testing_t *matmul_test
){

    // check
    if(matmul_test==NULL){
        return 1;
    }
    
    // free and set NULL
    if(matmul_test->matA!=NULL){
        free(matmul_test->matA);
    }else{
        printf(
            "[WARNING] " \
            "Function {matmul_free}, " \
            "line %d of file \"%s\", " \
            "is called upon a {matmul_testing_t} "\
            "without a {%s} buffer!",
            __LINE__, __FILE__, "matrixA" 
        );
    }
    if(matmul_test->matB!=NULL){
        free(matmul_test->matB);
    }else{
        printf(
            "[WARNING] " \
            "Function {matmul_free}, " \
            "line %d of file \"%s\", " \
            "is called upon a {matmul_testing_t} "\
            "without a {%s} buffer!",
            __LINE__, __FILE__, "matrixB" 
        );
    }
    if(matmul_test->matAB_res!=NULL){
        free(matmul_test->matAB_res);
    }else{
        printf(
            "[WARNING] " \
            "Function {matmul_free}, " \
            "line %d of file \"%s\", " \
            "is called upon a {matmul_testing_t} "\
            "without a {%s} buffer!",
            __LINE__, __FILE__, "matrixABres" 
        );
    }
    if(matmul_test->matAB_ref!=NULL){
        free(matmul_test->matAB_ref);
    }else{
        printf(
            "[WARNING] " \
            "Function {matmul_free}, " \
            "line %d of file \"%s\", " \
            "is called upon a {matmul_testing_t} "\
            "without a {%s} buffer!",
            __LINE__, __FILE__, "matrixABref" 
        );
    }

    // set sizes to 0
    matmul_test->mat_i = 0;
    matmul_test->mat_j = 0;
    matmul_test->mat_k = 0;
    matmul_test->matA = NULL;
    matmul_test->matB = NULL;
    matmul_test->matAB_res = NULL;
    matmul_test->matAB_ref = NULL;

    // return 0 if successful
    return 0;

}

/**************************************************************/
/******* MATMUL II - Initialisation ***************************/
/**************************************************************/

/* Methods of Init */
extern int mat_init0_random(float* mat_buf, int row, int col);
extern int mat_init1_unit(float* mat_buf, int row, int col);
extern int mat_init2_fillrow(float* mat_buf, int row, int col);
extern int mat_init3_fillcol(float* mat_buf, int row, int col);

/**************************************************************/

/*
Initialisation of MatrixA and MatrixB
 -- with 4 different options
     -- 0: random
     -- 1: unit matrix
     -- 2: natural number row-wise fill
     -- 3: natural number col-wise fill
*/
int matmul_init(
    struct matmul_testing_t *matmul_test,
    int init_a, int init_b
){

    // check existence of test
    if(matmul_test==NULL){
        return 1;
    }
    // check components
    if(
        matmul_test->matA==NULL ||
        matmul_test->matB==NULL ||
        matmul_test->matAB_ref==NULL ||
        matmul_test->matAB_res==NULL
    ){
        printf(
            "[ERROR] " \
            "Function {matmul_init}, " \
            "line %d of file \"%s\", " \
            "is called upon a {matmul_testing_t}"\
            "that is not properly created!",
            __LINE__, __FILE__
        );
        return 2;
    }

    
}

/**************************************************************/
/******* MATMUL III - Reference Solution **********************/
/**************************************************************/

/* Find reference result of matmul */
extern int matmul_ref_matmul(
    float* matA_buf, float* matB_buf, float* matC_buf, 
    int mat_i, int mat_j, int mat_k
);

/**************************************************************/

// Reference solution
int matmul_ref_result(
    struct matmul_testing_t *matmul_test
){

    // check
    if(matmul_test==NULL){
        return 1;
    }

    //


    return 0;
    
}

/**************************************************************/

// End of file
