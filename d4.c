// System includes
#include <stdio.h>
#include <stdlib.h>
// OpenCL includes
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#include "common.h"


void printMatrice(int n, int* matrice){
  for(int i = 0; i < n; ++i){
    for(int j=0; j < n; ++j){
      printf("%3d ", matrice[i * n + j]);
    }
    printf("\n");
  }
}

bool checkMatrice(int n, int* matrice){
  for(int i = 0; i < n; ++i){
    for(int j=0; j < n; ++j){
      if(matrice[i * n + j] > 2)
        return false;
    }
  }
  return true;
}

int* initMatrice(int n){
  int* matrice = calloc(n * n, sizeof(int));
  matrice[n-1] = 1;
  matrice[(n-1)*n] = 1;
  for(int i = 0; i < n; ++i){
    for(int j=0; j < n; ++j){
      if(i < n-1 && j < n-1 && i != j){
        matrice[i * n + j] = 1;
      }
      else if((i == n-1 || j == n-1) && i != 0 && j != 0 && i != j){
        matrice[i * n + j] = n+1;
      }
    }
  }
  return matrice;
}

const char* programSource =

"__kernel                                            \n"
"void floidWarshall(__global int *A,                       \n"
"            int k, int n)                                      \n"

"{                                                   \n"



"   // Get the work-item’s unique ID                \n"

"   int i = get_global_id(0);                      \n"
"   int j = get_global_id(1);               \n"
"     int ninja =             i *n +j;                  \n"
"     //if(ninja > n*n*99/100)    printf(\"Hello from %d\\n\", ninja);    \n"
"                                            \n"

"   A[n*i + j] = A[n*i + j] < A[n*i + k] + A[n*k + j] ? A[n*i + j] : A[n*i + k] + A[n*k + j];          \n"

"   //barrier(CLK_LOCAL_MEM_FENCE);    \n"

"  //for(int k=0; k< n;++k){                        \n"
"    //     A[n*i + j] = A[n*i + j] < A[n*i + k] + A[n*k + j] ? A[n*i + j] : A[n*i + k] + A[n*k + j];                    \n"
"                           \n"
"   //}   barrier(CLK_LOCAL_MEM_FENCE);                    \n"

"}                                                   \n"

;



int main() {

    int size = 2000; //1180 sur le IRIS ca plante
    int* A = NULL;
    A = initMatrice(size);
    size_t datasize = sizeof(int)*size*size;

    //printMatrice(size, A);
    //printf("\n");
    cl_int status;
    //-----------------------------------------------------
    // STEP 1: Discover and initialize the platforms
    //-----------------------------------------------------
    cl_uint numPlatforms = 0;
    cl_platform_id *platforms = NULL;
    // Use clGetPlatformIDs() to retrieve the number of
    // platforms
    status = clGetPlatformIDs(0, NULL, &numPlatforms);
    // Allocate enough space for each platform
    platforms = (cl_platform_id*)malloc(numPlatforms*sizeof(cl_platform_id));
    // Fill in platforms with clGetPlatformIDs()
    status = clGetPlatformIDs(numPlatforms, platforms, NULL);
    char Name[1000];
    clGetPlatformInfo(platforms[0], CL_PLATFORM_NAME, sizeof(Name), Name, NULL);
    //printf("Name of platform : %s\n", Name);
    //-----------------------------------------------------

    // STEP 2: Discover and initialize the devices

    //-----------------------------------------------------
    cl_uint numDevices = 0;
    cl_device_id *devices = NULL;
    // Use clGetDeviceIDs() to retrieve the number of
    // devices present
    status = clGetDeviceIDs(platforms[0],CL_DEVICE_TYPE_ALL,0,NULL,&numDevices);
    //printf("Number of devices = %d\n", (int)numDevices);
    // Allocate enough space for each device
    devices = (cl_device_id*)malloc(numDevices*sizeof(cl_device_id));
    // Fill in devices with clGetDeviceIDs()
    status = clGetDeviceIDs(platforms[0],CL_DEVICE_TYPE_ALL,numDevices,devices,NULL);
    for (int i=0; i<numDevices; i++){
        clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(Name),Name, NULL);
        printf("Name of device %d: %s\n", i, Name);

        char buffer[10240];
		    cl_uint buf_uint;
		    cl_ulong buf_ulong;
		    clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(buffer), buffer, NULL);
		printf("  DEVICE_NAME = %s\n", buffer);
		clGetDeviceInfo(devices[i], CL_DEVICE_VENDOR, sizeof(buffer), buffer, NULL);
		printf("  DEVICE_VENDOR = %s\n", buffer);
		clGetDeviceInfo(devices[i], CL_DEVICE_VERSION, sizeof(buffer), buffer, NULL);
		printf("  DEVICE_VERSION = %s\n", buffer);
		clGetDeviceInfo(devices[i], CL_DRIVER_VERSION, sizeof(buffer), buffer, NULL);
		printf("  DRIVER_VERSION = %s\n", buffer);
		clGetDeviceInfo(devices[i], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(buf_uint), &buf_uint, NULL);
		printf("  DEVICE_MAX_COMPUTE_UNITS = %u\n", (unsigned int)buf_uint);
		clGetDeviceInfo(devices[i], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(buf_uint), &buf_uint, NULL);
		printf("  DEVICE_MAX_CLOCK_FREQUENCY = %u\n", (unsigned int)buf_uint);
		clGetDeviceInfo(devices[i], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(buf_ulong), &buf_ulong, NULL);
		printf("  DEVICE_GLOBAL_MEM_SIZE = %llu\n", (unsigned long long)buf_ulong);

    clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(buf_ulong), &buf_ulong, NULL);
		printf("  CL_DEVICE_MAX_WORK_GROUP_SIZE = %llu\n", (unsigned long long)buf_ulong);
    clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(buf_ulong), &buf_ulong, NULL);
		printf("  CL_DEVICE_MAX_WORK_ITEM_SIZES = %llu\n", (unsigned long long)buf_ulong);
    clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(buf_ulong), &buf_ulong, NULL);
		printf("  CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS = %llu\n", (unsigned long long)buf_ulong);
    }
    //-----------------------------------------------------

    // STEP 3: Create a context

    //-----------------------------------------------------
    cl_context context = NULL;
    // Create a context using clCreateContext() and
    // associate it with the devices
    context = clCreateContext(NULL,numDevices,devices,NULL,NULL,&status);
    //-----------------------------------------------------

    // STEP 4: Create a command queue

    //-----------------------------------------------------
    cl_command_queue cmdQueue;
    // Create a command queue using clCreateCommandQueue(),
    // and associate it with the device you want to execute
    // on
    cmdQueue = clCreateCommandQueue(context,devices[1],0,&status);
    //-----------------------------------------------------

    // STEP 5: Create device buffers

    //-----------------------------------------------------
    cl_mem bufferA;

    // Use clCreateBuffer() to create a buffer object (d_A)
    // that will contain the data from the host array A
    bufferA = clCreateBuffer(context,CL_MEM_READ_WRITE,datasize,NULL,&status);
    if (status) printf("ERREUR A LA COMPILATION: %d\n", status);
    //-----------------------------------------------------

    // STEP 6: Write host data to device buffers

    //-----------------------------------------------------
    // Use clEnqueueWriteBuffer() to write input array A to
    // the device buffer bufferA
    status = clEnqueueWriteBuffer(cmdQueue,bufferA,CL_FALSE,0,datasize,A,0,NULL,NULL); // CL_FALSE

    //-----------------------------------------------------

    // STEP 7: Create and compile the program

    //-----------------------------------------------------

    // Create a program using clCreateProgramWithSource()
    cl_program program = clCreateProgramWithSource(context,1,(const char**)&programSource,NULL,&status);

    // Build (compile) the program for the devices with
    // clBuildProgram()
    status = clBuildProgram(program,numDevices,devices,NULL,NULL,NULL);
    if (status) printf("ERREUR A LA COMPILATION: %d\n", status);
    //-----------------------------------------------------

    // STEP 8: Create the kernel

    //-----------------------------------------------------
    cl_kernel kernel = NULL;
    // Use clCreateKernel() to create a kernel from the
    // vector addition function (named "vecadd")
    kernel = clCreateKernel(program, "floidWarshall", &status);
    //-----------------------------------------------------

    // STEP 9: Set the kernel arguments

    //-----------------------------------------------------
    // Associate the input and output buffers with the
    // kernel
    // using clSetKernelArg()

    status  = clSetKernelArg(kernel,0,sizeof(cl_mem),&bufferA);

    status |= clSetKernelArg(kernel, 2, sizeof(int), (void *)&size);
    if (status) printf("ERREUR ARG: %d\n", status);
    //-----------------------------------------------------

    // STEP 10: Configure the work-item structure

    //-----------------------------------------------------
    // Define an index space (global work size) of work
    // items for
    // execution. A workgroup size (local work size) is not
    // required,
    // but can be used.
    size_t globalWorkSize[2];
    // There are 'size' work-items
    globalWorkSize[0] = size;
    globalWorkSize[1] = size;
    //-----------------------------------------------------

    // STEP 11: Enqueue the kernel for execution

    //-----------------------------------------------------
    struct timespec time = get_time();
    // Execute the kernel by using
    // clEnqueueNDRangeKernel().
    // 'globalWorkSize' is the 1D dimension of the
    // work-items
    for(int k = 0; k<size; ++k){
      status = clSetKernelArg(kernel, 1, sizeof(int), (void *)&k);
      status = clEnqueueNDRangeKernel(cmdQueue,kernel,2,NULL,globalWorkSize,NULL,0,NULL,NULL);
    }
    //-----------------------------------------------------

    // STEP 12: Read the output buffer back to the host1

    //-----------------------------------------------------
    // Use clEnqueueReadBuffer() to read the OpenCL output
    // buffer (bufferC)
    // to the host output array (C)
    status = clEnqueueReadBuffer(cmdQueue,bufferA,CL_TRUE,0,datasize,A,0,NULL,NULL);
    struct timespec duration = get_duration(&time);
    print_time(&duration);
    if(checkMatrice(size, A))
      printf("\nOKAY\n");
    else
      printf("NOT OKAY\n");
    printf("\n");
    //printMatrice(size, A);

    //-----------------------------------------------------

    // STEP 13: Release OpenCL resources

    //-----------------------------------------------------
    // Free OpenCL resources
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(cmdQueue);
    clReleaseMemObject(bufferA);
    clReleaseContext(context);

    // Free host resources
    free(A);
    free(platforms);
    free(devices);
}
