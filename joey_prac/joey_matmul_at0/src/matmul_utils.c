/*
SOURCE
Parsing arguments to the matmul test executable!!
*/

/**************************************************************/

// Include its own header
#include "matmul_utils.h"

// Include stdio
#include <stdio.h>
#include <stdlib.h>

// Include argp
#include "argp.h"

/**************************************************************/
/******* Arg Parse I - Setting up argp ************************/
/**************************************************************/

// Set argp_options
static struct argp_option options[] = {
    /* OpenMP Options */
    { 
        .name = "omp",
        .key  = 100, 
        .arg  = 0,
        .flags = 0,
        .doc  = "Test the OpenMP Matmul"
    },
    { 
        .name = "ompkernel",
        .key  = 101, 
        .arg  = "kernel_id",
        .flags = 0,
        .doc  = "Set OpenMP Kernel"
    },
    { 
        .name = "ompnthr",
        .key  = 102, 
        .arg  = "number",
        .flags = 0,
        .doc  = "Set OpenMP Kernel"
    },
    /* OpenCL Options */
    { 
        .name = "ocl",
        .key  = 200, 
        .arg  = 0,
        .flags = 0,
        .doc  = "Test the OpenCL Matmul"
    },
    { 
        .name = "oclkernel",
        .key  = 201,
        .arg  = "kernel_id",
        .flags = 0,
        .doc  = "Set OpenCL Kernel"
    },
    { 
        .name = "ocldev",
        .key  = 202, 
        .arg  = "device_id",
        .flags = 0,
        .doc  = "Set #0 OpenCL Device (Optional)"
    },
    { 0 }
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
        // Toggle OpenMP tests
        case 100: {
            args->test_omp = 1;
            break;
        }
        // Kernel id
        case 101: {
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
        case 102: {
            char* end = NULL;
            long thread_num = strtol(
                arg, &end, 10
            );
            if(end!=arg){
                args->omp_thread_num=
                    (int)(thread_num);
            }
            break;
        }
        // Toggle OpenCL tests
        case 200: {
            args->test_ocl = 1;
            break;
        }
        // Kernel id
        case 201: {
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
        case 202: {
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

    // Init arguments
    args->test_omp=0;
    args->omp_kernel_id=-1;
    args->omp_thread_num=-1;
    args->test_ocl=0;
    args->ocl_device_id=-1;
    args->ocl_kernel_id=-1;

    // Parse arguments with argp
    error_t argp_err = 
        argp_parse(&argp, argc, argv, 0, NULL, args);
    
    // Debug thingies
    printf("[DEBUG] args.test_omp       = %d \n",
        args->test_omp);
    printf("[DEBUG] args.omp_kernel_id  = %d \n",
        args->omp_kernel_id);
    printf("[DEBUG] args.omp_thread_num = %d \n",
        args->omp_thread_num);
    printf("[DEBUG] args.test_ocl       = %d \n",
        args->test_ocl);
    printf("[DEBUG] args.ocl_kernel_id  = %d \n",
        args->ocl_kernel_id);
    printf("[DEBUG] args.ocl_device_id  = %d \n",
        args->ocl_device_id);

    return argp_err;

}

/**************************************************************/

// End of file
