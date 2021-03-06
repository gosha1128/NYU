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
#include "GPUkernel.hh"

// RAND
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include "cudarand.h"
#include "misc.cuh"

using namespace std;

// static paramaters
static CUfunction drvfunkSeedRandom;
static CUfunction drvfunf;

static int init = 0;

static GPUmat *gm;

static unsigned int hostRndMults[NUM_RND_STREAMS];
static bool rndInitialized;

//the driver API way
static CUdeviceptr devRndMults;
static CUdeviceptr devRndWords;

void initRandom(unsigned int seed) {
    assert(!rndInitialized);
    ifstream inFile;
    inFile.open(RND_MULTIPLIERS_FILE);
    if(!inFile) {
      /*std::cerr << "Unable to open file " << RND_MULTIPLIERS_FILE << std::endl;
        exit(EXIT_FAILURE);*/
      mexErrMsgTxt("Unable to open random seed file. Is it in the current directory?");

    }
    int numRead = 0;
    unsigned int mult;
    while(numRead < NUM_RND_STREAMS) {
        if(!(inFile >> mult)) {
	  /*std::cerr << "Not enough numbers in file " << RND_MULTIPLIERS_FILE << std::endl;
            exit(EXIT_FAILURE);*/
	  mexErrMsgTxt("Not enough numbers in random seed file.");

        }
        hostRndMults[numRead] = mult;
	//std::cout << "Read: " << hostRndMults[numRead];
        numRead++;
    }
    inFile.close();

    //the driverAPI way
    cuMemAlloc(&devRndMults, NUM_RND_STREAMS * sizeof(unsigned int));
    cuMemAlloc(&devRndWords, NUM_RND_STREAMS * sizeof(unsigned long long));
    cuMemcpyHtoD(devRndMults, hostRndMults, NUM_RND_STREAMS * sizeof(unsigned int));

    CUresult err = CUDA_SUCCESS;

    // setup execution parameters
    if (CUDA_SUCCESS != (err = cuFuncSetBlockShape(drvfunkSeedRandom, NUM_RND_THREADS_PER_BLOCK, 1, 1))) {
      mexErrMsgTxt("Error in cuFuncSetBlockShape");
    }

    /*    if (CUDA_SUCCESS != cuFuncSetSharedSize(drvfunkSeedRandom, 0)) {
	  mexErrMsgTxt("Error in cuFuncSetSharedSize");
	  }*/

    // add parameters
    int poffset = 0;

    // CUDA kernels interface
    // N: number of elements
    // offset: used for streams

    if (CUDA_SUCCESS
	!= cuParamSetv(drvfunkSeedRandom, poffset, &devRndMults, sizeof(devRndMults))) {
      mexErrMsgTxt("Error in cuParamSetv");
    }
    poffset += sizeof(devRndMults);

    if (CUDA_SUCCESS
	!= cuParamSetv(drvfunkSeedRandom, poffset, &devRndWords, sizeof(devRndWords))) {
      mexErrMsgTxt("Error in cuParamSetv");
    }
    poffset += sizeof(devRndWords);

    if (CUDA_SUCCESS != cuParamSeti(drvfunkSeedRandom, poffset, seed)) {
      mexErrMsgTxt("Error in cuParamSeti");
    }
    poffset += sizeof(seed);

    if (CUDA_SUCCESS != cuParamSetSize(drvfunkSeedRandom, poffset)) {
      mexErrMsgTxt("Error in cuParamSetSize");
    }

    err = cuLaunchGridAsync(drvfunkSeedRandom, NUM_RND_BLOCKS, 1, 0);
    if (CUDA_SUCCESS != err) {
      mexErrMsgTxt("Error running kernel");
    }

    rndInitialized = true;
}

void hostRandomGaussian(CUfunction drvfun, int N, float stdev, int nrhs, hostdrv_pars_t *prhs) {

    assert(rndInitialized);
    
    CUresult err = CUDA_SUCCESS;

    // setup execution parameters

    if (CUDA_SUCCESS != (err = cuFuncSetBlockShape(drvfun, NUM_RND_THREADS_PER_BLOCK, 1, 1))) {
      mexErrMsgTxt("Error in cuFuncSetBlockShape");
    }

    // add parameters
    int poffset = 0;

    // CUDA kernels interface
    // N: number of elements
    // offset: used for streams
    if (CUDA_SUCCESS != cuParamSetv(drvfun, poffset, &devRndMults, sizeof(devRndMults))) {
      mexErrMsgTxt("Error in cuParamSetv");
    }
    poffset += sizeof(devRndMults);

   if (CUDA_SUCCESS != cuParamSetv(drvfun, poffset, &devRndWords, sizeof(devRndWords))) {
      mexErrMsgTxt("Error in cuParamSetv");
    }
    poffset += sizeof(devRndWords);

    for (int p=0;p<nrhs;p++) {
      if (CUDA_SUCCESS
          != cuParamSetv(drvfun, poffset, prhs[p].par, prhs[p].psize)) {
        mexErrMsgTxt("Error in cuParamSetv");
      }
      poffset += prhs[p].psize;
    }

    /* Note that we pass in a pointer to the float
       but the offset is sizeof the actual float
       not sizeof the pointer */
   if (CUDA_SUCCESS != cuParamSetv(drvfun, poffset, &stdev, sizeof(stdev))) {
      mexErrMsgTxt("Error in cuParamSetv");
    }
    poffset += sizeof(stdev);
    

    if (CUDA_SUCCESS != cuParamSeti(drvfun, poffset, N)) {
      mexErrMsgTxt("Error in cuParamSeti");
    }
    poffset += sizeof(N);


    if (CUDA_SUCCESS != cuParamSetSize(drvfun, poffset)) {
      mexErrMsgTxt("Error in cuParamSetSize");
    }

    err = cuLaunchGridAsync(drvfun, NUM_RND_BLOCKS, 1, 0);
    if (CUDA_SUCCESS != err) {
      mexErrMsgTxt("Error running kernel");
    }

}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {

  CUresult cudastatus = CUDA_SUCCESS;

  if (nrhs < 2)
    mexErrMsgTxt("Expecting 2 or more RHS arguments.");

  if (init == 0) {
    // Initialize function
    mexLock();

    // load GPUmat
    gm = gmGetGPUmat();

    // load module
    CUmodule *drvmod = gmGetModule("misc");

    // load kRandomSeed kernel
    CUresult status = cuModuleGetFunction(&drvfunkSeedRandom, *drvmod, "kSeedRandom");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    // load RandomGaussian kernel
    status = cuModuleGetFunction(&drvfunf, *drvmod, "kRandomGaussian");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    //initialize random number generator with seed
    initRandom(4444);

    //used for debugging
    //mexPrintf("hostRndMults[0]: %u\n",hostRndMults[0]);
    //mexPrintf("hostRndMults[NUM_RND_STREAMS-1]: %u\n",hostRndMults[NUM_RND_STREAMS-1]);
    
    init=1;
  }


  // Dimensions are passed in
  // Last argument is always of type GPUtype
  // For now, only GPUSingle is supported
  // e.g. cuRrandn(3,4,5,GPUsingle)
  GPUtype IN = gm->gputype.getGPUtype(prhs[nrhs-1]);
  gpuTYPE_t tin = gm->gputype.getType(IN);

  CUfunction drvfun;
  if (tin == gpuFLOAT) 
    drvfun = drvfunf;
  else {
    mexErrMsgTxt("Only singles are supported at present.");
  }

  // we use an existing GPUmat that allows to create
  // a GPUtype with variable arguments, similar to the
  // Matlab syntax for rand function

  // nrhs-1 because last argument is a GPUtype
  // r is the returned output
  GPUtype r = gm->gputype.createMx(tin, nrhs-1, prhs);

  // number of elements
  int N = gm->gputype.getNumel(r);

  plhs[0] = gm->gputype.createMxArray(r);

  // I need the pointers to GPU memory
  CUdeviceptr d_OUT = (CUdeviceptr) (UINTPTR gm->gputype.getGPUptr(r));
  
  hostdrv_pars_t gpuprhs[1];
  int gpunrhs = 1;
  gpuprhs[0] = hostdrv_pars(&d_OUT,sizeof(d_OUT));

  //third param is std; fixed for now
  hostRandomGaussian(drvfun, N, 1.0f, gpunrhs, gpuprhs);
}
