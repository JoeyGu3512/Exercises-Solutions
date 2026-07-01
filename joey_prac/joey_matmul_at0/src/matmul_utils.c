/*
SOURCE
Parsing arguments to the matmul test executable!!
*/

/**************************************************************/

// Include its own header
#include "matmul_utils.h"

// Include stdio
#include <stdio.h>
// Inclide stdlib
#include <stdlib.h>

// Include argp
#include "argp.h"

// Debug toggle
#if 1
#ifndef DEBUG_MATMUL_PREPARE
#define DEBUG_MATMUL_PREPARE
#endif
#endif

/**************************************************************/
/******* Arg Parse I - Setting up argp ************************/
/**************************************************************/

// Set argp_options
static struct argp_option options[] = {
    /* Test Matrix Properties */
    { 
        .name = "???",
        .key  = 500,
        .arg  = 0,
        .flags = 0,
        .doc  = "???????"
    },
    { 
        .name = "mati",
        .key  = 501, 
        .arg  = "size",
        .flags = 0,
        .doc  = "Matsize[I]: A-row AB-row"
    },
    { 
        .name = "matj",
        .key  = 502, 
        .arg  = "size",
        .flags = 0,
        .doc  = "Matsize[J]: B-col AB-col"
    },
    { 
        .name = "matk",
        .key  = 503, 
        .arg  = "size",
        .flags = 0,
        .doc  = "Matsize[K]: A-col B-row"
    },
    { 
        .name = "initA",
        .key  = 504, 
        .arg  = "method",
        .flags = 0,
        .doc  = "Init MatA: \n" \
            "    0-random 1-unit 2-fillrow 3-fillcol"
    },
    { 
        .name = "initB",
        .key  = 505, 
        .arg  = "method",
        .flags = 0,
        .doc  = "Init MatB: \n" \
            "    0-random 1-unit 2-fillrow 3-fillcol"
    },
    /* OpenMP Options */
    { 
        .name = "omp",
        .key  = 200, 
        .arg  = 0,
        .flags = 0,
        .doc  = "Test the OpenMP Matmul"
    },
    { 
        .name = "ompkern",
        .key  = 201, 
        .arg  = "kernel_id",
        .flags = 0,
        .doc  = "Set OpenMP Kernel"
    },
    { 
        .name = "ompnthr",
        .key  = 202, 
        .arg  = "number",
        .flags = 0,
        .doc  = "Set OpenMP Kernel"
    },
    /* OpenCL Options */
    { 
        .name = "ocl",
        .key  = 300, 
        .arg  = 0,
        .flags = 0,
        .doc  = "Test the OpenCL Matmul"
    },
    { 
        .name = "oclkern",
        .key  = 301,
        .arg  = "kernel_id",
        .flags = 0,
        .doc  = "Set OpenCL Kernel"
    },
    { 
        .name = "ocldev",
        .key  = 302, 
        .arg  = "device_id",
        .flags = 0,
        .doc  = "Set #0 OpenCL Device (Optional)"
    },
    { 0 }
};

// Default values of args
static const struct arguments args_default = {
    .surprise=0,
    .mat_i=0,
    .mat_j=0,
    .mat_k=0,
    .mat_Ainit=0,
    .mat_Binit=0,
    .test_omp=0,
    .omp_kernel_id=-1,
    .omp_thread_num=-1,
    .test_ocl=0,
    .ocl_kernel_id=-1,
    .ocl_device_id=-1
};

// Parse
static error_t parse_opt(
    int key, char *arg, struct argp_state *state
){
    // Fetch the result struct
    struct arguments* args = state->input;

    /*
    Design reminder:
     -- toggles default to (0) meaning (false)
     -- all other default to (-1) meaning (default)
    */

    switch (key) {
        // Mat: ??? size-ijk initAB
        case 500: {
            args->surprise = 1;
            break;
        }
        case 501: {
            char* end = NULL;
            long size = strtol(
                arg, &end, 10
            );
            if(end!=arg){
                if(size<1){size=1;}    // min: 1
                args->mat_i=
                    (int)(size);
            }
            break;
        }
        case 502: {
            char* end = NULL;
            long size = strtol(
                arg, &end, 10
            );
            if(end!=arg){
                if(size<1){size=1;}    // min: 1
                args->mat_j=
                    (int)(size);
            }
            break;
        }
        case 503: {
            char* end = NULL;
            long size = strtol(
                arg, &end, 10
            );
            if(end!=arg){
                if(size<1){size=1;}    // min: 1
                args->mat_k=
                    (int)(size);
            }
            break;
        }
        case 504: {
            char* end = NULL;
            long init_method = strtol(
                arg, &end, 10
            );
            if(end!=arg){
                if(init_method<0 || init_method>3){
                    // if invalid, default to 0-random
                    init_method=0;    
                }
                args->mat_Ainit=
                    (int)(init_method);
            }
            break;
        }
        case 505: {
            char* end = NULL;
            long init_method = strtol(
                arg, &end, 10
            );
            if(end!=arg){
                if(init_method<0 || init_method>3){
                    // if invalid, default to 0-random
                    init_method=0;    
                }
                args->mat_Binit=
                    (int)(init_method);
            }
            break;
        }
        // Toggle OpenMP tests
        case 200: {
            args->test_omp = 1;
            break;
        }
        // Kernel id
        case 201: {
            char* end = NULL;
            long kernel_id = strtol(
                arg, &end, 10
            );
            if(end!=arg){
                args->omp_kernel_id=
                    (int)(kernel_id);
            }
            break;
        }
        // Number of omp threads
        case 202: {
            char* end = NULL;
            long thread_num = strtol(
                arg, &end, 10
            );
            if(end!=arg){
                if(thread_num<1){thread_num=1;}    // min: 1
                args->omp_thread_num=
                    (int)(thread_num);
            }
            break;
        }
        // Toggle OpenCL tests
        case 300: {
            args->test_ocl = 1;
            break;
        }
        // Kernel id
        case 301: {
            char* end = NULL;
            long kernel_id = strtol(
                arg, &end, 10
            );
            if(end!=arg){
                args->ocl_kernel_id=
                    (int)(kernel_id);
            }
            break;
        }
        // OpenCL device id
        case 302: {
            char* end = NULL;
            long device_id = strtol(
                arg, &end, 10
            );
            if(end!=arg){
                args->ocl_device_id=
                    (int)(device_id);
            }
            break;
        }
        default: 
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

// Assemble argp
static struct argp argp = {
    options,
    parse_opt,
    0,
    "Run OpenMP/OpenCL Matmul Practices..."
};

/**************************************************************/
/******* Arg Parse II - Function to be used *******************/
/**************************************************************/

// The function parsing arguments for matmul
error_t matmul_parse_args(
    int argc, char** argv,
    struct arguments* args
){

    // Write defaults (See "args_default" far above)
    *args = args_default;

    // Parse arguments with argp
    error_t argp_err = 
        argp_parse(&argp, argc, argv, 0, NULL, args);
    
    // Debug thingies
#ifdef DEBUG_ARG_PARSE
    printf("\n");
    printf("[DEBUG_ARG_PARSE] Debugging argument parsing~ \n");
    printf("[DEBUG_ARG_PARSE] file:        %s \n",
        __FILE__);
    printf("[DEBUG_ARG_PARSE] compiled:    %s - %s \n",
        __DATE__, __TIME__);
    printf("[DEBUG_ARG_PARSE] args.surprise       = %d \n",
        args->surprise);
    printf("[DEBUG_ARG_PARSE] args.mat_i          = %d \n",
        args->mat_i);
    printf("[DEBUG_ARG_PARSE] args.mat_j          = %d \n",
        args->mat_j);
    printf("[DEBUG_ARG_PARSE] args.mat_j          = %d \n",
        args->mat_k);
    printf("[DEBUG_ARG_PARSE] args.mat_Ainit      = %d \n",
        args->mat_Ainit);
    printf("[DEBUG_ARG_PARSE] args.mat_Binit      = %d \n",
        args->mat_Binit);
    printf("[DEBUG_ARG_PARSE] args.test_omp       = %d \n",
        args->test_omp);
    printf("[DEBUG_ARG_PARSE] args.omp_kernel_id  = %d \n",
        args->omp_kernel_id);
    printf("[DEBUG_ARG_PARSE] args.omp_thread_num = %d \n",
        args->omp_thread_num);
    printf("[DEBUG_ARG_PARSE] args.test_ocl       = %d \n",
        args->test_ocl);
    printf("[DEBUG_ARG_PARSE] args.ocl_kernel_id  = %d \n",
        args->ocl_kernel_id);
    printf("[DEBUG_ARG_PARSE] args.ocl_device_id  = %d \n",
        args->ocl_device_id);
    printf("\n");
#endif

    return argp_err;

}

/**************************************************************/

// End of file
