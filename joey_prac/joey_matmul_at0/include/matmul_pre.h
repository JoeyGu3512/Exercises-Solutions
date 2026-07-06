#ifndef MATMUL_PRE_H
#define MATMUL_PRE_H
/*
SOURCE
Parsing arguments to the matmul test executable!!
*/

/**************************************************************/

// Include Matmul definitions
#include "matmul.h"

/**************************************************************/

// Allocation
int matmul_alloc(
    struct matmul_testing_t **matmul_test,
    int mat_i, int mat_j, int mat_k
);
// Free
int matmul_free(
    struct matmul_testing_t **matmul_test
);
// Initialisation
int matmul_init(
    struct matmul_testing_t *matmul_test,
    int init_a, int init_b
);
// Reference solution
int matmul_ref_result(
    struct matmul_testing_t *matmul_test
);

/**************************************************************/


// End of file
#endif