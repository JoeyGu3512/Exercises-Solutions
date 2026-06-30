#ifndef ARG_PARSE_H
#define ARG_PARSE_H
/*
HEADER
Parsing arguments to the matmul test executable!!
*/

/**************************************************************/

// Struct of Results
struct arguments{
    /* ??? */
    int surprise;
    /* The Matrices */
    int mat_i;
    int mat_j;
    int mat_k;
    int mat_Ainit;
    int mat_Binit;
    /* OpenMP Settings */
    int test_omp;
    int omp_kernel_id;
    int omp_thread_num;
    /* OpenCL Settings */
    int test_ocl;
    int ocl_kernel_id;
    int ocl_device_id;
};

/**************************************************************/

// Parse argument for the matmul executable
int matmul_parse_args(
    int argc, char** argv,
    struct arguments* args
);

/**************************************************************/

// End of file
#endif