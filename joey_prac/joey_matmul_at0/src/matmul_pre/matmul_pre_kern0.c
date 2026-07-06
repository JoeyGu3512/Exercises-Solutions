/*
SOURCE
Parsing arguments to the matmul test executable!!
*/

/**************************************************************/

// Include stdio
#include <stdio.h>

// Include time
#include <time.h>

// Include OpenMP (just for speeding up a bit!)
#include <omp.h>

// Include GSL RNG
#include <gsl/gsl_rng.h>

// Define OpenMP shedule slide size
#define OMP_SCHEDULE_SIZE 128

/**************************************************************/

int mat_init0_random(
    float* restrict mat_buf, int row, int col
){

    // check
    if(mat_buf==NULL){
        return 1;
    }
    
    // Seed the stdlib rand()
    srand(
        (unsigned int)time(NULL)^(unsigned int)omp_get_wtime()
    );
    /* Suggested by DeepSeek, finer seeding. */

    // Finding end of loop
    int row_wkgr = 
        (row+OMP_SCHEDULE_SIZE-1)
        /OMP_SCHEDULE_SIZE *OMP_SCHEDULE_SIZE;
    int col_wkgr = 
        (col+OMP_SCHEDULE_SIZE-1)
        /OMP_SCHEDULE_SIZE *OMP_SCHEDULE_SIZE;

    // Manage random number
    int max_threads = omp_get_max_threads();
    gsl_rng** mat_rngs = (gsl_rng**)(
        malloc(max_threads*sizeof(void*))
    );
    // Init the rngs with rand() (LAZY!)
    for(int s=0;s<max_threads;s++){
        mat_rngs[s]=gsl_rng_alloc(gsl_rng_mt19937);
        gsl_rng_set(mat_rngs[s],(unsigned long)(rand()));
    }

    /*
    Uhhhh
    It just needs to be random so far.
    The seeds do not go outside this func 
    to pin the state anyway....
    Just gonna let omp do 
    whatever it likes with the rngs..
    OwO
    */

    // fill random
    #pragma omp parallel for collapse(2) schedule(static,OMP_SCHEDULE_SIZE)
    for(int i=0;i<row_wkgr;i++){
    for(int j=0;j<col_wkgr;j++){
    if(i<row && j<col){
        // thread number
        int thread_num = omp_get_thread_num();
        // random from -1 to +1
        double rand_ff = 
            gsl_rng_uniform(mat_rngs[thread_num]);
        mat_buf[i*col+j] = 
            2.0f*((float)(rand_ff))-1.0f;
    }
    }
    }

    // free the rngs
    for(int s=0;s<max_threads;s++){
        gsl_rng_free(mat_rngs[s]);
    }
    // free the ptr array of rngs
    free(mat_rngs);

    return 0;

}

/**************************************************************/

int mat_init1_unit(
    float* restrict mat_buf, int row, int col
){

    // check
    if(mat_buf==NULL){
        return 1;
    }

    // check size!!!
    if(row!=col){
        printf(
            "[INFO] " \
            "Function {mat_init1_unit}, " \
            "line %d of file \"%s\", " \
            "is called upon a matrix buffer, "\
            "yet the matrix size(%d,%d) is not square! \n",
            __LINE__, __FILE__, row, col
        );
    }

    // Finding end of loop
    int row_wkgr = 
        (row+OMP_SCHEDULE_SIZE-1)
        /OMP_SCHEDULE_SIZE *OMP_SCHEDULE_SIZE;
    int col_wkgr = 
        (col+OMP_SCHEDULE_SIZE-1)
        /OMP_SCHEDULE_SIZE *OMP_SCHEDULE_SIZE;

    // fill diag
    #pragma omp parallel for collapse(2) schedule(static,OMP_SCHEDULE_SIZE)
    for(int i=0;i<row_wkgr;i++){
    for(int j=0;j<row_wkgr;j++){
    if(i<row && j<col){
        // diag
        mat_buf[i*col+j] = (i==j) ? 1.0f : 0.0f;
    }
    }
    }


    return 0;
    
}

/**************************************************************/

int mat_init2_fillrow(
    float* restrict mat_buf, int row, int col
){

    // check
    if(mat_buf==NULL){
        return 1;
    }

    // Finding end of loop
    int row_wkgr = 
        (row+OMP_SCHEDULE_SIZE-1)
        /OMP_SCHEDULE_SIZE *OMP_SCHEDULE_SIZE;
    int col_wkgr = 
        (col+OMP_SCHEDULE_SIZE-1)
        /OMP_SCHEDULE_SIZE *OMP_SCHEDULE_SIZE;

    // fill natu row
    #pragma omp parallel for collapse(2) schedule(static,OMP_SCHEDULE_SIZE)
    for(int i=0;i<row_wkgr;i++){
    for(int j=0;j<row_wkgr;j++){
    if(i<row && j<col){
        // natu row
        mat_buf[i*col+j] = (i*col+j);
    }
    }
    }


    return 0;
    
}

/**************************************************************/

int mat_init3_fillcol(
    float* restrict mat_buf, int row, int col
){

    // check
    if(mat_buf==NULL){
        return 1;
    }

    // Finding end of loop
    int row_wkgr = 
        (row+OMP_SCHEDULE_SIZE-1)
        /OMP_SCHEDULE_SIZE *OMP_SCHEDULE_SIZE;
    int col_wkgr = 
        (col+OMP_SCHEDULE_SIZE-1)
        /OMP_SCHEDULE_SIZE *OMP_SCHEDULE_SIZE;

    // fill natu row
    #pragma omp parallel for collapse(2) schedule(static,OMP_SCHEDULE_SIZE)
    for(int i=0;i<row_wkgr;i++){
    for(int j=0;j<row_wkgr;j++){
    if(i<row && j<col){
        // natu row
        mat_buf[i*col+j] = (j*row+i);
    }
    }
    }

    return 0;
    
}

/**************************************************************/

// end of file
