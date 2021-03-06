#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifdef UNIX
#include <stdint.h>
#endif

#include "mex.h"


// CUDA
#include "cuda.h"
#include "cuda_runtime.h"

#include "GPUmat.hh"



// static paramaters
static int init = 0;
static GPUmat *gm;

/*
 * This function prints information about the input GPUtype
 */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {

  CUresult cudastatus = CUDA_SUCCESS;

  // At least 1 argument expected
  if (nrhs!=1)
     mexErrMsgTxt("Wrong number of arguments");

  if (init == 0) {
    // Initialize function

    // load GPUmat
    gm = gmGetGPUmat();

    // load module
    // NOT REQUIRED

    // load float GPU function
    // NOT REQUIRED

    init = 1;
  }



  // mex parameters are:
  // IN1: GPUtype


  GPUtype IN1  = gm->gputype.getGPUtype(prhs[0]);

  /* ndims is the number of dimensions */
  int ndims = gm->gputype.getNdims(IN1);
  /* s is the array of dimensions */
  const int *s = gm->gputype.getSize(IN1);
  gpuTYPE_t type = gm->gputype.getType(IN1);

  mexPrintf("GPUtype properties\n", ndims);

  if (type==gpuFLOAT) {
    mexPrintf("  Type = single/real\n");
  } else if (type==gpuCFLOAT) {
    mexPrintf("  Type = single/complex\n");
  } else if (type==gpuDOUBLE) {
    mexPrintf("  Type = double/real\n");
  } else if (type==gpuCDOUBLE) {
    mexPrintf("  Type = double/complex\n");
  }

  mexPrintf("  Number of dimensions = %d\n", ndims);
  mexPrintf("  Size = (", ndims);

  for (int i=0;i<ndims;i++) {
    mexPrintf(" %d ", s[i]);
  }
  mexPrintf(")\n");

  mexPrintf("  Pointer to GPU memory = %p\n", gm->gputype.getGPUptr(IN1));
  mexPrintf("  Number of elements    = %d\n", gm->gputype.getNumel(IN1));
  mexPrintf("  Size of data (bytes)  = %d\n", gm->gputype.getDataSize(IN1));


}
