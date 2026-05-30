//------------------------------------------------------------------------------
//
// Name:       vadd.c
//
// Purpose:    Elementwise addition of two vectors (c = a + b)
//
// HISTORY:    Written by Tim Mattson, December 2009
//             Updated by Tom Deakin and Simon McIntosh-Smith, October 2012
//             Updated by Tom Deakin, July 2013
//             Updated by Tom Deakin, October 2014
//
//------------------------------------------------------------------------------


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#include <unistd.h>
#else
#include <CL/cl.h>
#endif

#include "err_code.h"

//pick up device type from compiler command line or from
//the default type
#ifndef DEVICE
#define DEVICE CL_DEVICE_TYPE_DEFAULT
#endif


extern double wtime();       // returns time since some fixed past point (wtime.c)
extern int output_device_info(cl_device_id );


//------------------------------------------------------------------------------

#define TOL    (0.001)   // tolerance used in floating point comparisons
#define LENGTH (1024)    // length of vectors a, b, and c

//------------------------------------------------------------------------------
//
// kernel:  vadd
//
// Purpose: Compute the elementwise sum c = a+b
//
// input: a and b float vectors of length count
//
// output: c float vector of length count holding the sum a + b
//

const char *KernelSource = "\n" \
"__kernel void vadd(                                                 \n" \
"   __global float* a,                                                  \n" \
"   __global float* b,                                                  \n" \
"   __global float* c,                                                  \n" \
"   const unsigned int count)                                           \n" \
"{                                                                      \n" \
"   int i = get_global_id(0);                                           \n" \
"   if(i < count)                                                       \n" \
"       c[i] = a[i] + b[i];                                             \n" \
"}                                                                      \n" \
"\n";

//------------------------------------------------------------------------------


int main(int argc, char** argv)
{

    //---------------------------------------------------//

    // Preparations

    int err;               // error code returned from OpenCL calls

    float* h_a = (float*) calloc(LENGTH, sizeof(float));
    float* h_b = (float*) calloc(LENGTH, sizeof(float));
    float* h_d = (float*) calloc(LENGTH, sizeof(float));
    float* h_f = (float*) calloc(LENGTH, sizeof(float));
    float* h_g = (float*) calloc(LENGTH, sizeof(float));    // result

    unsigned int correct;           // number of correct results

    size_t global;                  // global domain size

    cl_device_id     device_id;     // compute device id
    cl_context       context;       // compute context
    cl_command_queue commands;      // compute command queue
    cl_program       program;       // compute program
    cl_kernel        ko_vadd;       // compute kernel

    /*
    Chain add 
      - Nagging: Why chain??? Reduction?
    */
    cl_mem d_a;
    cl_mem d_b;
    cl_mem d_c;    // c = a+b
    cl_mem d_d;
    cl_mem d_e;    // e = c+d    
    cl_mem d_f; 
    cl_mem d_g;    // g = e+f

    // Fill vectors a and b with random float values
    int i = 0;
    int count = LENGTH;
    for(i = 0; i < count; i++){
        h_a[i] = rand() / (float)RAND_MAX;
        h_b[i] = rand() / (float)RAND_MAX;
        h_d[i] = rand() / (float)RAND_MAX;
        h_f[i] = rand() / (float)RAND_MAX;
    }

    //---------------------------------------------------//

    // Set up platform and GPU device
    cl_uint numPlatforms;

    // Find number of platforms
    err = clGetPlatformIDs(0, NULL, &numPlatforms);
    checkError(err, "Finding platforms");
    if (numPlatforms == 0)
    {
        printf("Found 0 platforms!\n");
        return EXIT_FAILURE;
    }

    // Get all platforms
    cl_platform_id Platform[numPlatforms];
    err = clGetPlatformIDs(numPlatforms, Platform, NULL);
    checkError(err, "Getting platforms");

    // Secure a GPU
    for (i = 0; i < numPlatforms; i++)
    {
        err = clGetDeviceIDs(Platform[i], DEVICE, 1, &device_id, NULL);
        if (err == CL_SUCCESS)
        {
            break;
        }
    }

    if (device_id == NULL)
        checkError(err, "Finding a device");

    err = output_device_info(device_id);
    checkError(err, "Printing device output");

    //---------------------------------------------------//

    // Create a compute context
    context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
    checkError(err, "Creating context");

    //---------------------------------------------------//

    // Create a command queue
    commands = clCreateCommandQueueWithProperties(
        context, device_id, 
        NULL, &err
    );
    checkError(err, "Creating command queue");

    //---------------------------------------------------//

    // Create the compute program from the source buffer
    program = clCreateProgramWithSource(context, 1, (const char **) & KernelSource, NULL, &err);
    checkError(err, "Creating program");

    // Build the program
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        size_t len;
        char buffer[2048];

        printf("Error: Failed to build program executable!\n%s\n", err_code(err));
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
        return EXIT_FAILURE;
    }

    // Create the compute kernel from the program
    ko_vadd = clCreateKernel(program, "vadd", &err);
    checkError(err, "Creating kernel");

    //---------------------------------------------------//

    // Create the input (a, b) and output (c) arrays in device memory
    d_a  = clCreateBuffer(context,
        CL_MEM_READ_ONLY,  
        sizeof(float) * count, 
        NULL, &err);
    checkError(err, "Creating buffer d_a");

    d_b  = clCreateBuffer(context,
        CL_MEM_READ_ONLY,  
        sizeof(float) * count, 
        NULL, &err);
    checkError(err, "Creating buffer d_b");

    d_c  = clCreateBuffer(context,
        CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS, 
        sizeof(float) * count, 
        NULL, &err);
    checkError(err, "Creating buffer d_c");

    d_d  = clCreateBuffer(context,
        CL_MEM_READ_ONLY, 
        sizeof(float) * count, 
        NULL, &err);
    checkError(err, "Creating buffer d_d");

    d_e  = clCreateBuffer(context,
        CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS, 
        sizeof(float) * count, 
        NULL, &err);
    checkError(err, "Creating buffer d_e");

    d_f  = clCreateBuffer(context,
        CL_MEM_READ_ONLY, 
        sizeof(float) * count, 
        NULL, &err);
    checkError(err, "Creating buffer d_f");

    d_g  = clCreateBuffer(context,
        CL_MEM_WRITE_ONLY, 
        sizeof(float) * count, 
        NULL, &err);
    checkError(err, "Creating buffer d_g");

    //---------------------------------------------------//

    // Buffer write events
    cl_event events_writing_buffer[4];

    // Write abdf vectors into compute device memory
    err = clEnqueueWriteBuffer(commands, 
        d_a, CL_FALSE,
        0, sizeof(float) * count, h_a, 
        0, NULL, &events_writing_buffer[0]);
    checkError(err, "Copying h_a to device at d_a");

    err = clEnqueueWriteBuffer(commands, 
        d_b, CL_FALSE, 
        0, sizeof(float) * count, h_b, 
        0, NULL, &events_writing_buffer[1]);
    checkError(err, "Copying h_b to device at d_b");

    err = clEnqueueWriteBuffer(commands, 
        d_d, CL_FALSE, 
        0, sizeof(float) * count, h_d, 
        0, NULL, &events_writing_buffer[2]);
    checkError(err, "Copying h_d to device at d_d");

    err = clEnqueueWriteBuffer(commands, 
        d_f, CL_FALSE, 
        0, sizeof(float) * count, h_f, 
        0, NULL, &events_writing_buffer[3]);
    checkError(err, "Copying h_f to device at d_f");

    //---------------------------------------------------//

    double rtime = wtime();

    //---------------------------------------------------//

    // kernel run dependencies
    cl_event kernel1_deps[2];
    cl_event kernel2_deps[2];
    cl_event kernel3_deps[2];
    // buffer write deps
    kernel1_deps[0] = events_writing_buffer[0];
    kernel1_deps[1] = events_writing_buffer[1];
    kernel2_deps[0] = events_writing_buffer[2];
    kernel3_deps[0] = events_writing_buffer[3];

    /* (kernel:C=A+B) - dep on (Awrite,Bwrite) */
    kernel1_deps[0] = events_writing_buffer[0];
    kernel1_deps[1] = events_writing_buffer[1];
    // Set the arguments to our compute kernel
    err  = clSetKernelArg(ko_vadd, 0, sizeof(cl_mem), &d_a);
    err |= clSetKernelArg(ko_vadd, 1, sizeof(cl_mem), &d_b);
    err |= clSetKernelArg(ko_vadd, 2, sizeof(cl_mem), &d_c);
    err |= clSetKernelArg(ko_vadd, 3, sizeof(unsigned int), &count);
    checkError(err, "Setting kernel arguments");
    // Execute the kernel over the entire range of our 1d input data set
    // letting the OpenCL runtime choose the work-group size
    global = count;
    err = clEnqueueNDRangeKernel(commands, ko_vadd,
        // wk_dim, glb_offset, glb_size, local_size 
        1, NULL, &global, NULL, 
        // wait for a and b buffer write
        2, kernel1_deps,
        // kernel event
        &kernel2_deps[1]);
    checkError(err, "Enqueueing (kernel:C=A+B)");

    /* E = C+D - dep on (Dwrite,kernel1) */
    // Set the arguments to our compute kernel
    err  = clSetKernelArg(ko_vadd, 0, sizeof(cl_mem), &d_c);
    err |= clSetKernelArg(ko_vadd, 1, sizeof(cl_mem), &d_d);
    err |= clSetKernelArg(ko_vadd, 2, sizeof(cl_mem), &d_e);
    err |= clSetKernelArg(ko_vadd, 3, sizeof(unsigned int), &count);
    checkError(err, "Setting kernel arguments");
    // Execute the kernel over the entire range of our 1d input data set
    // letting the OpenCL runtime choose the work-group size
    global = count;
    err = clEnqueueNDRangeKernel(commands, ko_vadd,
        // wk_dim, glb_offset, glb_size, local_size 
        1, NULL, &global, NULL, 
        // wait for a and b buffer write
        2, kernel2_deps,
        // kernel event
        &kernel3_deps[1]);
    checkError(err, "Enqueueing (kernel:E=C+D)");

    /* G = E+F - dep on (Fwrite,kernel2) */
    // Set the arguments to our compute kernel
    err  = clSetKernelArg(ko_vadd, 0, sizeof(cl_mem), &d_e);
    err |= clSetKernelArg(ko_vadd, 1, sizeof(cl_mem), &d_f);
    err |= clSetKernelArg(ko_vadd, 2, sizeof(cl_mem), &d_g);
    err |= clSetKernelArg(ko_vadd, 3, sizeof(unsigned int), &count);
    checkError(err, "Setting kernel arguments");
    // Execute the kernel over the entire range of our 1d input data set
    // letting the OpenCL runtime choose the work-group size
    global = count;
    err = clEnqueueNDRangeKernel(commands, ko_vadd,
        // wk_dim, glb_offset, glb_size, local_size 
        1, NULL, &global, NULL, 
        // wait for a and b buffer write
        2, kernel3_deps,
        // kernel event
        NULL);    // end of queue just wait for finish
    checkError(err, "Enqueueing (kernel:G=E+F)");

    //---------------------------------------------------//

    // Wait for the commands to complete before stopping the timer
    err = clFinish(commands);
    checkError(err, "Waiting for kernel to finish");

    //---------------------------------------------------//

    rtime = wtime() - rtime;
    printf("\nThe kernel ran in %lf seconds\n",rtime);

    //---------------------------------------------------//

    // Read back the results from the compute device
    err = clEnqueueReadBuffer(
        // queue, device_buffer, block?
        commands, d_g, CL_TRUE,
        // offset, size, host_buffer
        0, sizeof(float) * count, h_g,
        // num_wait, wait_list, this_event
        0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to read output array!\n%s\n", err_code(err));
        exit(1);
    }

    //---------------------------------------------------//

    // Test the results
    correct = 0;
    float tmp;

    for(i = 0; i < count; i++)
    {
        tmp = h_a[i] + h_b[i] + h_d[i] + h_f[i];
        tmp -= h_g[i];
        if(tmp*tmp < TOL*TOL)
            correct++;
        else {
            printf(
                " tmp %f h_g %f h_a %f h_b %f h_d %f h_f %f \n",
                tmp, h_g[i], h_a[i], h_b[i], h_d[i], h_f[i]
            );
        }
    }

    //---------------------------------------------------//

    // summarise results
    printf(
        "G=A+B+D+F:  %d out of %d results were correct.\n", 
        correct, count
    );

    //---------------------------------------------------//

    // cleanup then shutdown
    clReleaseMemObject(d_a);
    clReleaseMemObject(d_b);
    clReleaseMemObject(d_c);
    clReleaseMemObject(d_d);
    clReleaseMemObject(d_e);
    clReleaseMemObject(d_f);
    clReleaseMemObject(d_g);
    clReleaseProgram(program);
    clReleaseKernel(ko_vadd);
    clReleaseCommandQueue(commands);
    clReleaseContext(context);

    free(h_a);
    free(h_b);
    free(h_d);
    free(h_f);

    //---------------------------------------------------//

    return 0;

}

