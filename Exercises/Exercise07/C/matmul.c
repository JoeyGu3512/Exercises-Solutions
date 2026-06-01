//------------------------------------------------------------------------------
//
//  PROGRAM: Matrix Multiplication driver
//
//  PURPOSE: This is a driver program to test various ways of computing
//           the product:
//
//                C  = A * B
//
//           A and B are set to constant matrices so we
//           can make a quick test of the multiplication.
//
//  USAGE:   The matrices are constant matrices, square and the order is
//           set as a constant, ORDER (see mult.h).
//
//  HISTORY: Written by Tim Mattson, August 2010 
//           Modified by Simon McIntosh-Smith, September 2011
//           Modified by Tom Deakin and Simon McIntosh-Smith, October 2012
//           Ported to C by Tom Deakin, July 2013
//           Modified to assume square matrices by Simon McIntosh-Smith, Sep 2014
//
//------------------------------------------------------------------------------

#include "matmul.h"
#include "matrix_lib.h"
#include "err_code.h"
#include "device_picker.h"

#define NUM_VER 4
#define GLOBAL_ROUND_UP(ng,nl) ((((ng)+(nl)-1)/(nl))*nl)

#define KERNEL1_LOCAL_WG 32
#define KERNEL2_LOCAL_WG 32
#define KERNEL3_LOCAL_WG 32

const char *joey_kernel_src_arr[NUM_VER] = {
    "./matmul_brute01_naive.cl",
    "./matmul_brute02_private.cl",
    "./matmul_brute03_cacheBcol.cl",
    "./matmul_brute04_cachefullBcol.cl"
};

const size_t kernels_local_ndrange[NUM_VER][2] = {
    {1,1},
    {KERNEL1_LOCAL_WG,1},
    {KERNEL2_LOCAL_WG,1},
    {KERNEL3_LOCAL_WG,1}
};

const size_t kernels_global_ndrange[NUM_VER][2] = {
    {ORDER,ORDER},
    {
        GLOBAL_ROUND_UP(ORDER,KERNEL1_LOCAL_WG),
        1
    },
    {
        GLOBAL_ROUND_UP(ORDER,KERNEL2_LOCAL_WG),
        GLOBAL_ROUND_UP(ORDER,KERNEL2_LOCAL_WG)
    },
    {
        GLOBAL_ROUND_UP(ORDER,KERNEL3_LOCAL_WG),
        GLOBAL_ROUND_UP(ORDER,KERNEL3_LOCAL_WG)
    }
};

int main(int argc, char *argv[]){

    //----------------------------------------------------//

    char *endptr;
    int kernel_version = (int)(strtol(argv[1], &endptr, 10));

    if (*endptr != '\0') {
        printf(
            "Conversion error: non-numeric characters found: %s\n", 
            endptr
        );
        return 1;
    }

    if(kernel_version>=NUM_VER){
        printf(
            "Kernel idx outbound, only %d versions available! \n", 
            (int)(NUM_VER)
        );
        return 1;
    }

    //----------------------------------------------------//

    float *h_A;             // A matrix
    float *h_B;             // B matrix
    float *h_C;             // C = A*B matrix
    float *h_Cref;             // Cref = A*B matrix from omp
    int N;                  // A[N][N], B[N][N], C[N][N]
    int size;               // number of elements in each matrix

    cl_mem d_a, d_b, d_c;   // Matrices in device memory

    double start_time;      // Starting time
    double run_time;        // timing data

    cl_int err;             // error code returned from OpenCL calls
    cl_device_id     device;     // compute device id 
    cl_context       context;       // compute context
    cl_command_queue commands;      // compute command queue
    cl_program       program;       // compute program
    cl_kernel        kernel;        // compute kernel

    N    = ORDER;
    size = N * N;

    h_A = (float *)malloc(size * sizeof(float));
    h_B = (float *)malloc(size * sizeof(float));
    h_C = (float *)malloc(size * sizeof(float));
    h_Cref = (float *)malloc(size * sizeof(float));



//--------------------------------------------------------------------------------
// Create a context, queue and device.
//--------------------------------------------------------------------------------

    cl_uint deviceIndex = 0;
    parseArguments(argc, argv, &deviceIndex);

    // Get list of devices
    cl_device_id devices[MAX_DEVICES];
    unsigned numDevices = getDeviceList(devices);

    // Check device index in range
    if (deviceIndex >= numDevices)
    {
      printf("Invalid device index (try '--list')\n");
      return EXIT_FAILURE;
    }

    device = devices[deviceIndex];

    char name[MAX_INFO_STRING];
    getDeviceName(device, name);
    printf("\nUsing OpenCL device: %s\n", name);

    // Create a compute context
    context = clCreateContext(0, 1, &device, NULL, NULL, &err);
    checkError(err, "Creating context");

   // Create a command queue
    commands = clCreateCommandQueueWithProperties(
        context, device, NULL, &err);
    checkError(err, "Creating command queue");

//--------------------------------------------------------------------------------
// Run sequential version on the host
//--------------------------------------------------------------------------------

    initmat(N, h_A, h_B, h_C);

    printf("\n===== Sequential, matrix mult (dot prod), order %d on host CPU ======\n",ORDER);
    for(int i = 0; i < COUNT; i++)
    {
        zero_mat(N, h_Cref);
        start_time = wtime();

        seq_mat_mul_sdot(N, h_A, h_B, h_Cref);

        run_time  = wtime() - start_time;
        results(N, h_Cref, run_time);
    }


//--------------------------------------------------------------------------------
// Setup the buffers, initialize matrices, and write them into global memory
//--------------------------------------------------------------------------------

    //  Reset A, B and C matrices (just to play it safe)
    // initmat(N, h_A, h_B, h_C);

    d_a = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                            sizeof(float) * size, h_A, &err);
    checkError(err, "Creating buffer d_a");

    d_b = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                            sizeof(float) * size, h_B, &err);
    checkError(err, "Creating buffer d_b");

    d_c = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
                            sizeof(float) * size, NULL, &err);
    checkError(err, "Creating buffer d_c");

//--------------------------------------------------------------------------------
// OpenCL matrix multiplication ... Naive
//--------------------------------------------------------------------------------

    // Load my kernel
    char mykernelsrc[4096];
    char *mykernelsrcptr[1];
    mykernelsrcptr[0] = &mykernelsrc[0];
    {
        FILE *mykernelsrcfile = fopen(
            joey_kernel_src_arr[kernel_version],"rb"
        );
        if(mykernelsrcfile==NULL){
            printf("Faield to open kernel source file!!!");
            return 1;
        }
        size_t read_status = fread(
            &mykernelsrc, sizeof(char), 4096, mykernelsrcfile);
        if(read_status==0){ 
            printf("Failed to load kernel source!!!");
            return 1;
        }
        fclose(mykernelsrcfile);
        mykernelsrc[read_status] = '\0';
    }


    // Create the comput program from the source buffer
    program = clCreateProgramWithSource(
        context, 
        1,(const char**)(mykernelsrcptr), NULL, &err);
    checkError(err, "Creating program");

   // Build the program
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        size_t len;
        char buffer[2048];

        printf("Error: Failed to build program executable!\n%s\n", err_code(err));
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
        return EXIT_FAILURE;
    }

    // Create the compute kernel from the program
    kernel = clCreateKernel(program, "mmul", &err);
    checkError(err, "Creating kernel");

    printf("\n===== OpenCL, matrix mult, C(i,j) per work item, order %d ======\n",N);

    // Do the multiplication COUNT times
    for (int i = 0; i < COUNT; i++)
    {
        zero_mat(N, h_C);

        err =  clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_a);
        err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_b);
        err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_c);
        err |= clSetKernelArg(kernel, 3, sizeof(int)   , &N  );
        checkError(err, "Setting kernel arguments");

        start_time = wtime();

        // Execute the kernel over the entire range of C matrix elements ... computing
        // a dot product for each element of the product matrix.  The local work
        // group size is set to NULL ... so I'm telling the OpenCL runtime to
        // figure out a local work group size for me.
        size_t local[2];
        local[0] = kernels_local_ndrange[kernel_version][0];
        local[1] = kernels_local_ndrange[kernel_version][1];
        size_t global[2] = {
            ((N+local[0]-1)/local[0])*local[0],
            ((N+local[1]-1)/local[1])*local[1]
        };
        err = clEnqueueNDRangeKernel(
            commands, kernel,
            2, NULL, 
            kernels_global_ndrange[kernel_version], 
            kernels_local_ndrange[kernel_version],
            0, NULL, NULL);

        checkError(err, "Enqueuing kernel");

        err = clFinish(commands);
        checkError(err, "Waiting for commands to finish");

        run_time = wtime() - start_time;

        err = clEnqueueReadBuffer(
            commands, d_c, CL_TRUE, 0,
            sizeof(float) * size, h_C,
            0, NULL, NULL);
        checkError(err, "Reading back buffer d_c");

        results(N, h_C, run_time);

    } // end for loop

    

    // Test the results
    int correct = 0;
    float tmp;

    for(int i = 0; i < N*N; i++)
    {
        tmp = h_Cref[i];
        tmp -= h_C[i];
        if(tmp*tmp < TOL*TOL)
            correct++;
        else {
            printf(
                " h_C %f h_Cref %f \n",
                h_C[i], h_Cref[i]);
        }
    }

    // summarise results
    printf(
        "Matmul: %d out of %d results were correct.\n", 
        correct, N*N);


//--------------------------------------------------------------------------------
// Clean up!
//--------------------------------------------------------------------------------

    free(h_A);
    free(h_B);
    free(h_C);
    free(h_Cref);
    clReleaseMemObject(d_a);
    clReleaseMemObject(d_b);
    clReleaseMemObject(d_c);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(commands);
    clReleaseContext(context);

    return EXIT_SUCCESS;
}
