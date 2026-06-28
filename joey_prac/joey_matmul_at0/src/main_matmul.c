/*
Program for playing with matmuls
JoeyGu3412QwQ
2026June27
*/

/**************************************************************/

// Include std stuff
#include <stdio.h>
#include <stdlib.h>

// Include DS's timing macro header
#include "timing.h"

// Include utils header
#include "matmul_utils.h"

// Include omp ref impl of matmul
#include "matmul_omp_ref.h"
// Include omp impl
#include "matmul_omp.h"
// Include ocl impl
#include "matmul_ocl.h"

/**************************************************************/

int main(int argc, char** argv){

    // Parse args
    struct arguments args;
    matmul_parse_args(argc,argv,&args);

    return 0;
    
}

/**************************************************************/

// End of program 🍎
