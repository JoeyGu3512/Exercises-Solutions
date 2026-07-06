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
    struct matmul_testing_t **matmul_test_ptr,
    int mat_i, int mat_j, int mat_k
){

    // check ptr
    if(matmul_test_ptr==NULL){
        return 1;
    }

    // check existence first
    if(*matmul_test_ptr!=NULL){
        printf(
            "[Error] The matmul test is already created. " \
            "Free it first to recreate! \n"
        );
        return 2;
    }

    // alloc struct!
    struct matmul_testing_t* matmul_test = 
        (struct matmul_testing_t*)(
            malloc(sizeof(struct matmul_testing_t))
        );
    if(matmul_test==NULL){
        printf(
            "[Error] Failed to allocate memory for the " \
            "matmul testing struct!!! \n"
        );
    }
    // store the ptr
    *matmul_test_ptr = matmul_test;

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
            (int)(matA_size*sizeof(float))
        );
        return 3;
    }

    host_buffer_B = malloc(matB_size*sizeof(float));
    if(host_buffer_B==NULL){
        printf(
            "[ERROR] Failed to allocate " \
            "%d Bytes for {host_buffer_B}! \n",
            (int)(matB_size*sizeof(float))
        );
        free(host_buffer_A); host_buffer_A=NULL;
        return 3;
    }
    
    host_buffer_ABres = malloc(matAB_size*sizeof(float));
    if(host_buffer_ABres==NULL){
        printf(
            "[ERROR] Failed to allocate " \
            "%d Bytes for {host_buffer_ABresult}! \n",
            (int)(matAB_size*sizeof(float))
        );
        free(host_buffer_A); host_buffer_A=NULL;
        free(host_buffer_B); host_buffer_B=NULL;
        return 3;
    }
    
    host_buffer_ABref = malloc(matAB_size*sizeof(float));
    if(host_buffer_ABref==NULL){
        printf(
            "[ERROR] Failed to allocate " \
            "%d Bytes for {host_buffer_ABreference}! \n",
            (int)(matAB_size*sizeof(float))
        );
        free(host_buffer_A); host_buffer_A=NULL;
        free(host_buffer_B); host_buffer_B=NULL;
        free(host_buffer_ABres); host_buffer_ABres=NULL;
        return 3;
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
    struct matmul_testing_t **matmul_test_ptr
){

    // check ptr
    if(matmul_test_ptr==NULL){
        return 1;
    }
    
    // store the pointer for now
    struct matmul_testing_t* matmul_test = 
        *matmul_test_ptr;

    // check
    if(matmul_test==NULL){
        printf(
            "[Error] The matmul test is non-existent. " \
            "Nothing to free! \n"
        );
        return 2;
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
            "without a {%s} buffer! \n",
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
            "without a {%s} buffer! \n",
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
            "without a {%s} buffer! \n",
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
            "without a {%s} buffer! \n",
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

    // free the matmul testing struct itself
    free(*matmul_test_ptr);
    *matmul_test_ptr = NULL;


    // return 0 if successful
    return 0;

}

/**************************************************************/
/******* MATMUL II - Initialisation ***************************/
/**************************************************************/

/* Methods of Init */
extern int mat_init0_random(
    float* restrict mat_buf, int row, int col
);
extern int mat_init1_unit(
    float* restrict mat_buf, int row, int col
);
extern int mat_init2_fillrow(
    float* restrict mat_buf, int row, int col
);
extern int mat_init3_fillcol(
    float* restrict mat_buf, int row, int col
);

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
            "that is not properly created! \n",
            __LINE__, __FILE__
        );
        return 2;
    }

    /* Init MatA */
    int init_code_a = 0;
    switch(init_a){
        case 0:
            init_code_a = mat_init0_random(
                matmul_test->matA,
                matmul_test->mat_i, matmul_test->mat_k
            );
            break;
        case 1:
            init_code_a = mat_init1_unit(
                matmul_test->matA,
                matmul_test->mat_i, matmul_test->mat_k
            );
            break;
        case 2:
            init_code_a = mat_init2_fillrow(
                matmul_test->matA,
                matmul_test->mat_i, matmul_test->mat_k
            );
            break;
        case 3:
            init_code_a = mat_init3_fillcol(
                matmul_test->matA,
                matmul_test->mat_i, matmul_test->mat_k
            );
            break;
        default:
            init_code_a = mat_init0_random(
                matmul_test->matA,
                matmul_test->mat_i, matmul_test->mat_k
            );
            break;
            
    }

    /* Init MatB */
    int init_code_b = 0;
    switch(init_b){
        case 0:
            init_code_b = mat_init0_random(
                matmul_test->matB,
                matmul_test->mat_k, matmul_test->mat_j
            );
            break;
        case 1:
            init_code_b = mat_init1_unit(
                matmul_test->matB,
                matmul_test->mat_k, matmul_test->mat_j
            );
            break;
        case 2:
            init_code_b = mat_init2_fillrow(
                matmul_test->matB,
                matmul_test->mat_k, matmul_test->mat_j
            );
            break;
        case 3:
            init_code_b = mat_init3_fillcol(
                matmul_test->matB,
                matmul_test->mat_k, matmul_test->mat_j
            );
            break;
        default:
            init_code_b = mat_init0_random(
                matmul_test->matB,
                matmul_test->mat_k, matmul_test->mat_j
            );
            break;
    }


    // return
    int return_code = 0;
    if(init_code_a!=0){ return_code += (int)(1)<<0; }
    if(init_code_b!=0){ return_code += (int)(1)<<1; }

    return 0;
    
}

/**************************************************************/
/******* MATMUL III - Reference Solution **********************/
/**************************************************************/

/* Find reference result of matmul */
extern int matmul_ref_matmul(
    float* restrict matA_buf, 
    float* restrict matB_buf, 
    float* restrict matC_buf, 
    int mat_i, int mat_j, int mat_k,
    const float alpha,
    const float beta
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

    // find ref result!!
    int ret_val = 
        matmul_ref_matmul(
            matmul_test->matA,
            matmul_test->matB,
            matmul_test->matAB_ref,
            matmul_test->mat_i,
            matmul_test->mat_j,
            matmul_test->mat_k,
            1.0f, 0.0f
        );
    if(ret_val!=0){
        return 2;
    }


    return 0;
    
}

/**************************************************************/

// End of file
