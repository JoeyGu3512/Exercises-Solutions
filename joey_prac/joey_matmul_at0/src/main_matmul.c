/*
Program for playing with matmuls
JoeyGu3412QwQ
2026June27
*/

/**************************************************************/

// Include std stuff
#include <stdio.h>
#include <stdlib.h>

// Include utils header
#include "matmul.h"

/* Components */
#include "matmul_utils.h"
#include "matmul_pre.h"
#include "matmul_test_omp.h"
#include "matmul_test_ocl.h"

// Include tiny utils
#include "timing.h"
#include "print_error_code.h"

/**************************************************************/

int main(int argc, char** argv){

    /******************************************************/
    /******* [0] Parse Argument ***************************/
    /******************************************************/

    // starting!!
    printf("[INFO] Matmul testing starting... \n");
    fflush(stdout);

    // parse args
    struct arguments args;
    matmul_parse_args(argc,argv,&args);

    // print some params - matsizes
    printf("[INFO] Matsizes: (%d,%d)(%d,%d)->(%d,%d) \n",
        args.mat_i, args.mat_k,
        args.mat_k, args.mat_j,
        args.mat_i, args.mat_j
    );
    fflush(stdout);

    /******************************************************/
    /******* [1.0] Prepare Mats - Allocate ****************/
    /******************************************************/

    // return codes
    int ret_code = 0;
    // handle to matmul testing 
    struct matmul_testing_t* matmul_test = NULL;

    // allocate
    ret_code = matmul_alloc(
        &matmul_test,
        args.mat_i, args.mat_j, args.mat_k
    ); 
    if(ret_code!=0){ 
        PRINT_ERROR_CODE("matmul_alloc",ret_code); 
        return 1; 
    }
    printf("[INFO] Matmul testing is created. \n");
    fflush(stdout);

    /******************************************************/
    /******* [1.1] Prepare Mats - Fill A & B **************/
    /******************************************************/

    // init 
    ret_code = matmul_init(
        matmul_test,
        args.mat_Ainit, args.mat_Binit
    );  
    if(ret_code!=0){ 
        PRINT_ERROR_CODE("matmul_init",ret_code); 
        return 1; 
    }
    printf("[INFO] Matmul testing is initialised. \n");
    fflush(stdout);

    /******************************************************/
    /******* [1.2] Prepare Mats - Reference Result ********/
    /******************************************************/

    // finding ref result
    ret_code = matmul_ref_result(matmul_test);  
    if(ret_code!=0){ 
        PRINT_ERROR_CODE("matmul_init",ret_code); 
        return 1; 
    }
    printf(
        "[INFO] Reference result computed (with OpenBLAS). \n");
    fflush(stdout);

    /******************************************************/
    /******* [*] Finalize *********************************/
    /******************************************************/

    // free 
    ret_code = matmul_free(&matmul_test);
    if(ret_code!=0){ 
        PRINT_ERROR_CODE("matmul_free",ret_code); 
        return 1; 
    }
    printf("[INFO] Matmul testing is destroyed. \n");
    fflush(stdout);

    /******************************************************/

    return 0;
    
}

/**************************************************************/

// End of program 🍎
