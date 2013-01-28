/*!
 * @brief This function does the valid convolutions between at set of feature maps
 * and corresponding images convolving each possible pais of feautre maps and
 * input map in a set for each numCases sets of them..
 *
 * This only works on square images and maps.
 * outputSize = mapSize - imSize + 1; in each diemsnsion of the planes. 
 *
 * @file
 * @author Matthew Zeiler (zeiler@cs.nyu.edu)
 * @data Apr 29, 2011
 *
 * @pooltoolbox_file @copybrief cuConv5.cpp
 * @gpu_file @copybrief cuConv5.cpp
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

//added to support floor, sqrt
#include <math.h>
//added to support assert
#include <assert.h>

#ifdef UNIX
#include <stdint.h>
#endif

#include "mex.h"

// CUDA
#include "cuda.h"
#include "cuda_runtime.h"

#include "GPUmat.hh"

#include "conv.cuh"

/* Static function definitions for each instantiation of kernel Emacs
numbers in registers is super helpful for creating this repetitive
code:
http://www.emacswiki.org/emacs/NumbersInRegisters
*/


/*!
 * @copybrief cuConv5.cpp
 *
 * @param MAPS   	      prhs[0] // The stack of feature maps should be numPixels x numFeatureMaps*numCases.
 * @param IMAGES          prhs[1] // The set of filters should be imgPixels x numInputMaps*numCases
 * @param OUTPUT          prhs[2] // The output maps should be outPixels x numInputMaps*numFeatureMaps*numCases.
 * @param numGroups       prhs[3] // The number of groups should be numInputMaps*numCases as summation in a group is over numFeatureMaps.
 * @param color           prhs[4] // Leave at 0
 * @param imgOrder        prhs[5] // Leave at 0.
 *
 * @retval OUTPUT   	  prhs[0] // The output maps of size outPixels x numInputMaps*numFeatureMaps*numCases.
 */

static CUfunction conv5_bw_nofit_16x16_F_F_1_F;
static CUfunction conv5_bw_nofit_16x16_F_F_1_T;
static CUfunction conv5_bw_nofit_16x16_F_F_3_F;
static CUfunction conv5_bw_nofit_16x16_F_F_3_T;
static CUfunction conv5_bw_nofit_16x16_F_T_1_F;
static CUfunction conv5_bw_nofit_16x16_F_T_1_T;
static CUfunction conv5_bw_nofit_16x16_F_T_3_F;
static CUfunction conv5_bw_nofit_16x16_F_T_3_T;
static CUfunction conv5_bw_nofit_16x16_T_F_1_F;
static CUfunction conv5_bw_nofit_16x16_T_F_1_T;
static CUfunction conv5_bw_nofit_16x16_T_F_3_F;
static CUfunction conv5_bw_nofit_16x16_T_F_3_T;
static CUfunction conv5_bw_nofit_16x16_T_T_1_F;
static CUfunction conv5_bw_nofit_16x16_T_T_1_T;
static CUfunction conv5_bw_nofit_16x16_T_T_3_F;
static CUfunction conv5_bw_nofit_16x16_T_T_3_T;

static CUfunction conv5_bw_fit_16x16_2_F_1_F;
static CUfunction conv5_bw_fit_16x16_2_F_1_T;
static CUfunction conv5_bw_fit_16x16_2_F_3_F;
static CUfunction conv5_bw_fit_16x16_2_F_3_T;
static CUfunction conv5_bw_fit_16x16_2_T_1_F;
static CUfunction conv5_bw_fit_16x16_2_T_1_T;
static CUfunction conv5_bw_fit_16x16_2_T_3_F;
static CUfunction conv5_bw_fit_16x16_2_T_3_T;

static CUfunction conv5_bw_fit_16x16_3_F_1_F;
static CUfunction conv5_bw_fit_16x16_3_F_1_T;
static CUfunction conv5_bw_fit_16x16_3_F_3_F;
static CUfunction conv5_bw_fit_16x16_3_F_3_T;
static CUfunction conv5_bw_fit_16x16_3_T_1_F;
static CUfunction conv5_bw_fit_16x16_3_T_1_T;
static CUfunction conv5_bw_fit_16x16_3_T_3_F;
static CUfunction conv5_bw_fit_16x16_3_T_3_T;

static CUfunction conv5_bw_fit_16x16_4_F_1_F;
static CUfunction conv5_bw_fit_16x16_4_F_1_T;
static CUfunction conv5_bw_fit_16x16_4_F_3_F;
static CUfunction conv5_bw_fit_16x16_4_F_3_T;
static CUfunction conv5_bw_fit_16x16_4_T_1_F;
static CUfunction conv5_bw_fit_16x16_4_T_1_T;
static CUfunction conv5_bw_fit_16x16_4_T_3_F;
static CUfunction conv5_bw_fit_16x16_4_T_3_T;

static CUfunction conv5_bw_fit_16x16_5_F_1_F;
static CUfunction conv5_bw_fit_16x16_5_F_1_T;
static CUfunction conv5_bw_fit_16x16_5_F_3_F;
static CUfunction conv5_bw_fit_16x16_5_F_3_T;
static CUfunction conv5_bw_fit_16x16_5_T_1_F;
static CUfunction conv5_bw_fit_16x16_5_T_1_T;
static CUfunction conv5_bw_fit_16x16_5_T_3_F;
static CUfunction conv5_bw_fit_16x16_5_T_3_T;

static CUfunction conv5_bw_fit_16x16_6_F_1_F;
static CUfunction conv5_bw_fit_16x16_6_F_1_T;
static CUfunction conv5_bw_fit_16x16_6_F_3_F;
static CUfunction conv5_bw_fit_16x16_6_F_3_T;
static CUfunction conv5_bw_fit_16x16_6_T_1_F;
static CUfunction conv5_bw_fit_16x16_6_T_1_T;
static CUfunction conv5_bw_fit_16x16_6_T_3_F;
static CUfunction conv5_bw_fit_16x16_6_T_3_T;

static CUfunction conv5_bw_fit_16x16_7_F_1_F;
static CUfunction conv5_bw_fit_16x16_7_F_1_T;
static CUfunction conv5_bw_fit_16x16_7_F_3_F;
static CUfunction conv5_bw_fit_16x16_7_F_3_T;
static CUfunction conv5_bw_fit_16x16_7_T_1_F;
static CUfunction conv5_bw_fit_16x16_7_T_1_T;
static CUfunction conv5_bw_fit_16x16_7_T_3_F;
static CUfunction conv5_bw_fit_16x16_7_T_3_T;

static CUfunction conv5_bw_fit_16x16_8_F_1_F;
static CUfunction conv5_bw_fit_16x16_8_F_1_T;
static CUfunction conv5_bw_fit_16x16_8_F_3_F;
static CUfunction conv5_bw_fit_16x16_8_F_3_T;
static CUfunction conv5_bw_fit_16x16_8_T_1_F;
static CUfunction conv5_bw_fit_16x16_8_T_1_T;
static CUfunction conv5_bw_fit_16x16_8_T_3_F;
static CUfunction conv5_bw_fit_16x16_8_T_3_T;

static CUfunction conv5_bw_fit_16x16_9_F_1_F;
static CUfunction conv5_bw_fit_16x16_9_F_1_T;
static CUfunction conv5_bw_fit_16x16_9_F_3_F;
static CUfunction conv5_bw_fit_16x16_9_F_3_T;
static CUfunction conv5_bw_fit_16x16_9_T_1_F;
static CUfunction conv5_bw_fit_16x16_9_T_1_T;
static CUfunction conv5_bw_fit_16x16_9_T_3_F;
static CUfunction conv5_bw_fit_16x16_9_T_3_T;

static CUfunction conv5_bw_fit_16x16_10_F_1_F;
static CUfunction conv5_bw_fit_16x16_10_F_1_T;
static CUfunction conv5_bw_fit_16x16_10_F_3_F;
static CUfunction conv5_bw_fit_16x16_10_F_3_T;
static CUfunction conv5_bw_fit_16x16_10_T_1_F;
static CUfunction conv5_bw_fit_16x16_10_T_1_T;
static CUfunction conv5_bw_fit_16x16_10_T_3_F;
static CUfunction conv5_bw_fit_16x16_10_T_3_T;

static CUfunction conv5_bw_fit_16x16_11_F_1_F;
static CUfunction conv5_bw_fit_16x16_11_F_1_T;
static CUfunction conv5_bw_fit_16x16_11_F_3_F;
static CUfunction conv5_bw_fit_16x16_11_F_3_T;
static CUfunction conv5_bw_fit_16x16_11_T_1_F;
static CUfunction conv5_bw_fit_16x16_11_T_1_T;
static CUfunction conv5_bw_fit_16x16_11_T_3_F;
static CUfunction conv5_bw_fit_16x16_11_T_3_T;

static CUfunction conv5_bw_fit_16x16_12_F_1_F;
static CUfunction conv5_bw_fit_16x16_12_F_1_T;
static CUfunction conv5_bw_fit_16x16_12_F_3_F;
static CUfunction conv5_bw_fit_16x16_12_F_3_T;
static CUfunction conv5_bw_fit_16x16_12_T_1_F;
static CUfunction conv5_bw_fit_16x16_12_T_1_T;
static CUfunction conv5_bw_fit_16x16_12_T_3_F;
static CUfunction conv5_bw_fit_16x16_12_T_3_T;

static CUfunction conv5_bw_fit_16x16_13_F_1_F;
static CUfunction conv5_bw_fit_16x16_13_F_1_T;
static CUfunction conv5_bw_fit_16x16_13_F_3_F;
static CUfunction conv5_bw_fit_16x16_13_F_3_T;
static CUfunction conv5_bw_fit_16x16_13_T_1_F;
static CUfunction conv5_bw_fit_16x16_13_T_1_T;
static CUfunction conv5_bw_fit_16x16_13_T_3_F;
static CUfunction conv5_bw_fit_16x16_13_T_3_T;

static CUfunction conv5_bw_fit_16x16_14_F_1_F;
static CUfunction conv5_bw_fit_16x16_14_F_1_T;
static CUfunction conv5_bw_fit_16x16_14_F_3_F;
static CUfunction conv5_bw_fit_16x16_14_F_3_T;
static CUfunction conv5_bw_fit_16x16_14_T_1_F;
static CUfunction conv5_bw_fit_16x16_14_T_1_T;
static CUfunction conv5_bw_fit_16x16_14_T_3_F;
static CUfunction conv5_bw_fit_16x16_14_T_3_T;

static CUfunction conv5_bw_fit_16x16_15_F_1_F;
static CUfunction conv5_bw_fit_16x16_15_F_1_T;
static CUfunction conv5_bw_fit_16x16_15_F_3_F;
static CUfunction conv5_bw_fit_16x16_15_F_3_T;
static CUfunction conv5_bw_fit_16x16_15_T_1_F;
static CUfunction conv5_bw_fit_16x16_15_T_1_T;
static CUfunction conv5_bw_fit_16x16_15_T_3_F;
static CUfunction conv5_bw_fit_16x16_15_T_3_T;

static CUfunction conv5_bw_fit_16x16_16_F_1_F;
static CUfunction conv5_bw_fit_16x16_16_F_1_T;
static CUfunction conv5_bw_fit_16x16_16_F_3_F;
static CUfunction conv5_bw_fit_16x16_16_F_3_T;
static CUfunction conv5_bw_fit_16x16_16_T_1_F;
static CUfunction conv5_bw_fit_16x16_16_T_1_T;
static CUfunction conv5_bw_fit_16x16_16_T_3_F;
static CUfunction conv5_bw_fit_16x16_16_T_3_T;

static CUfunction conv5_bw_fit_16x16_17_F_1_F;
static CUfunction conv5_bw_fit_16x16_17_F_1_T;
static CUfunction conv5_bw_fit_16x16_17_F_3_F;
static CUfunction conv5_bw_fit_16x16_17_F_3_T;
static CUfunction conv5_bw_fit_16x16_17_T_1_F;
static CUfunction conv5_bw_fit_16x16_17_T_1_T;
static CUfunction conv5_bw_fit_16x16_17_T_3_F;
static CUfunction conv5_bw_fit_16x16_17_T_3_T;

static CUfunction conv5_bw_fit_16x16_18_F_1_F;
static CUfunction conv5_bw_fit_16x16_18_F_1_T;
static CUfunction conv5_bw_fit_16x16_18_F_3_F;
static CUfunction conv5_bw_fit_16x16_18_F_3_T;
static CUfunction conv5_bw_fit_16x16_18_T_1_F;
static CUfunction conv5_bw_fit_16x16_18_T_1_T;
static CUfunction conv5_bw_fit_16x16_18_T_3_F;
static CUfunction conv5_bw_fit_16x16_18_T_3_T;

static CUfunction conv5_bw_fit_16x16_19_F_1_F;
static CUfunction conv5_bw_fit_16x16_19_F_1_T;
static CUfunction conv5_bw_fit_16x16_19_F_3_F;
static CUfunction conv5_bw_fit_16x16_19_F_3_T;
static CUfunction conv5_bw_fit_16x16_19_T_1_F;
static CUfunction conv5_bw_fit_16x16_19_T_1_T;
static CUfunction conv5_bw_fit_16x16_19_T_3_F;
static CUfunction conv5_bw_fit_16x16_19_T_3_T;

static CUfunction conv5_bw_fit_16x16_20_F_1_F;
static CUfunction conv5_bw_fit_16x16_20_F_1_T;
static CUfunction conv5_bw_fit_16x16_20_F_3_F;
static CUfunction conv5_bw_fit_16x16_20_F_3_T;
static CUfunction conv5_bw_fit_16x16_20_T_1_F;
static CUfunction conv5_bw_fit_16x16_20_T_1_T;
static CUfunction conv5_bw_fit_16x16_20_T_3_F;
static CUfunction conv5_bw_fit_16x16_20_T_3_T;

static CUfunction conv5_bw_fit_16x16_21_F_1_F;
static CUfunction conv5_bw_fit_16x16_21_F_1_T;
static CUfunction conv5_bw_fit_16x16_21_F_3_F;
static CUfunction conv5_bw_fit_16x16_21_F_3_T;
static CUfunction conv5_bw_fit_16x16_21_T_1_F;
static CUfunction conv5_bw_fit_16x16_21_T_1_T;
static CUfunction conv5_bw_fit_16x16_21_T_3_F;
static CUfunction conv5_bw_fit_16x16_21_T_3_T;

static CUfunction conv5_bw_fit_16x16_22_F_1_F;
static CUfunction conv5_bw_fit_16x16_22_F_1_T;
static CUfunction conv5_bw_fit_16x16_22_F_3_F;
static CUfunction conv5_bw_fit_16x16_22_F_3_T;
static CUfunction conv5_bw_fit_16x16_22_T_1_F;
static CUfunction conv5_bw_fit_16x16_22_T_1_T;
static CUfunction conv5_bw_fit_16x16_22_T_3_F;
static CUfunction conv5_bw_fit_16x16_22_T_3_T;

static CUfunction conv5_bw_fit_16x16_23_F_1_F;
static CUfunction conv5_bw_fit_16x16_23_F_1_T;
static CUfunction conv5_bw_fit_16x16_23_F_3_F;
static CUfunction conv5_bw_fit_16x16_23_F_3_T;
static CUfunction conv5_bw_fit_16x16_23_T_1_F;
static CUfunction conv5_bw_fit_16x16_23_T_1_T;
static CUfunction conv5_bw_fit_16x16_23_T_3_F;
static CUfunction conv5_bw_fit_16x16_23_T_3_T;

static CUfunction conv5_bw_fit_16x16_24_F_1_F;
static CUfunction conv5_bw_fit_16x16_24_F_1_T;
static CUfunction conv5_bw_fit_16x16_24_F_3_F;
static CUfunction conv5_bw_fit_16x16_24_F_3_T;
static CUfunction conv5_bw_fit_16x16_24_T_1_F;
static CUfunction conv5_bw_fit_16x16_24_T_1_T;
static CUfunction conv5_bw_fit_16x16_24_T_3_F;
static CUfunction conv5_bw_fit_16x16_24_T_3_T;

static CUfunction conv5_bw_fit_16x16_25_F_1_F;
static CUfunction conv5_bw_fit_16x16_25_F_1_T;
static CUfunction conv5_bw_fit_16x16_25_F_3_F;
static CUfunction conv5_bw_fit_16x16_25_F_3_T;
static CUfunction conv5_bw_fit_16x16_25_T_1_F;
static CUfunction conv5_bw_fit_16x16_25_T_1_T;
static CUfunction conv5_bw_fit_16x16_25_T_3_F;
static CUfunction conv5_bw_fit_16x16_25_T_3_T;

static CUfunction conv5_bw_fit_16x16_26_F_1_F;
static CUfunction conv5_bw_fit_16x16_26_F_1_T;
static CUfunction conv5_bw_fit_16x16_26_F_3_F;
static CUfunction conv5_bw_fit_16x16_26_F_3_T;
static CUfunction conv5_bw_fit_16x16_26_T_1_F;
static CUfunction conv5_bw_fit_16x16_26_T_1_T;
static CUfunction conv5_bw_fit_16x16_26_T_3_F;
static CUfunction conv5_bw_fit_16x16_26_T_3_T;

static CUfunction conv5_bw_fit_16x16_27_F_1_F;
static CUfunction conv5_bw_fit_16x16_27_F_1_T;
static CUfunction conv5_bw_fit_16x16_27_F_3_F;
static CUfunction conv5_bw_fit_16x16_27_F_3_T;
static CUfunction conv5_bw_fit_16x16_27_T_1_F;
static CUfunction conv5_bw_fit_16x16_27_T_1_T;
static CUfunction conv5_bw_fit_16x16_27_T_3_F;
static CUfunction conv5_bw_fit_16x16_27_T_3_T;

static CUfunction conv5_bw_fit_16x16_28_F_1_F;
static CUfunction conv5_bw_fit_16x16_28_F_1_T;
static CUfunction conv5_bw_fit_16x16_28_F_3_F;
static CUfunction conv5_bw_fit_16x16_28_F_3_T;
static CUfunction conv5_bw_fit_16x16_28_T_1_F;
static CUfunction conv5_bw_fit_16x16_28_T_1_T;
static CUfunction conv5_bw_fit_16x16_28_T_3_F;
static CUfunction conv5_bw_fit_16x16_28_T_3_T;

static CUfunction conv5_bw_fit_16x16_29_F_1_F;
static CUfunction conv5_bw_fit_16x16_29_F_1_T;
static CUfunction conv5_bw_fit_16x16_29_F_3_F;
static CUfunction conv5_bw_fit_16x16_29_F_3_T;
static CUfunction conv5_bw_fit_16x16_29_T_1_F;
static CUfunction conv5_bw_fit_16x16_29_T_1_T;
static CUfunction conv5_bw_fit_16x16_29_T_3_F;
static CUfunction conv5_bw_fit_16x16_29_T_3_T;

static CUfunction conv5_bw_fit_16x16_30_F_1_F;
static CUfunction conv5_bw_fit_16x16_30_F_1_T;
static CUfunction conv5_bw_fit_16x16_30_F_3_F;
static CUfunction conv5_bw_fit_16x16_30_F_3_T;
static CUfunction conv5_bw_fit_16x16_30_T_1_F;
static CUfunction conv5_bw_fit_16x16_30_T_1_T;
static CUfunction conv5_bw_fit_16x16_30_T_3_F;
static CUfunction conv5_bw_fit_16x16_30_T_3_T;

static CUfunction conv5_bw_fit_16x16_31_F_1_F;
static CUfunction conv5_bw_fit_16x16_31_F_1_T;
static CUfunction conv5_bw_fit_16x16_31_F_3_F;
static CUfunction conv5_bw_fit_16x16_31_F_3_T;
static CUfunction conv5_bw_fit_16x16_31_T_1_F;
static CUfunction conv5_bw_fit_16x16_31_T_1_T;
static CUfunction conv5_bw_fit_16x16_31_T_3_F;
static CUfunction conv5_bw_fit_16x16_31_T_3_T;

static CUfunction conv5_bw_fit_16x16_32_F_1_F;
static CUfunction conv5_bw_fit_16x16_32_F_1_T;
static CUfunction conv5_bw_fit_16x16_32_F_3_F;
static CUfunction conv5_bw_fit_16x16_32_F_3_T;
static CUfunction conv5_bw_fit_16x16_32_T_1_F;
static CUfunction conv5_bw_fit_16x16_32_T_1_T;
static CUfunction conv5_bw_fit_16x16_32_T_3_F;
static CUfunction conv5_bw_fit_16x16_32_T_3_T;

static CUfunction conv5_bw_fit_16x16_33_F_1_F;
static CUfunction conv5_bw_fit_16x16_33_F_1_T;
static CUfunction conv5_bw_fit_16x16_33_F_3_F;
static CUfunction conv5_bw_fit_16x16_33_F_3_T;
static CUfunction conv5_bw_fit_16x16_33_T_1_F;
static CUfunction conv5_bw_fit_16x16_33_T_1_T;
static CUfunction conv5_bw_fit_16x16_33_T_3_F;
static CUfunction conv5_bw_fit_16x16_33_T_3_T;

static CUfunction conv5_bw_fit_16x16_34_F_1_F;
static CUfunction conv5_bw_fit_16x16_34_F_1_T;
static CUfunction conv5_bw_fit_16x16_34_F_3_F;
static CUfunction conv5_bw_fit_16x16_34_F_3_T;
static CUfunction conv5_bw_fit_16x16_34_T_1_F;
static CUfunction conv5_bw_fit_16x16_34_T_1_T;
static CUfunction conv5_bw_fit_16x16_34_T_3_F;
static CUfunction conv5_bw_fit_16x16_34_T_3_T;

static CUfunction conv5_bw_fit_16x16_35_F_1_F;
static CUfunction conv5_bw_fit_16x16_35_F_1_T;
static CUfunction conv5_bw_fit_16x16_35_F_3_F;
static CUfunction conv5_bw_fit_16x16_35_F_3_T;
static CUfunction conv5_bw_fit_16x16_35_T_1_F;
static CUfunction conv5_bw_fit_16x16_35_T_1_T;
static CUfunction conv5_bw_fit_16x16_35_T_3_F;
static CUfunction conv5_bw_fit_16x16_35_T_3_T;

static CUfunction conv5_bw_fit_16x16_36_F_1_F;
static CUfunction conv5_bw_fit_16x16_36_F_1_T;
static CUfunction conv5_bw_fit_16x16_36_F_3_F;
static CUfunction conv5_bw_fit_16x16_36_F_3_T;
static CUfunction conv5_bw_fit_16x16_36_T_1_F;
static CUfunction conv5_bw_fit_16x16_36_T_1_T;
static CUfunction conv5_bw_fit_16x16_36_T_3_F;
static CUfunction conv5_bw_fit_16x16_36_T_3_T;

static CUfunction conv5_bw_fit_16x16_37_F_1_F;
static CUfunction conv5_bw_fit_16x16_37_F_1_T;
static CUfunction conv5_bw_fit_16x16_37_F_3_F;
static CUfunction conv5_bw_fit_16x16_37_F_3_T;
static CUfunction conv5_bw_fit_16x16_37_T_1_F;
static CUfunction conv5_bw_fit_16x16_37_T_1_T;
static CUfunction conv5_bw_fit_16x16_37_T_3_F;
static CUfunction conv5_bw_fit_16x16_37_T_3_T;

static int init = 0;

static GPUmat *gm;

/* Define wrappers for each of Alex's convolution kernels
   Since GPUmat uses the driver API it's easier to write these wrappers and put the driver API-related calls in here */
void _conv5_bw_fit_16x16(CUfunction drvfun, void* images, unsigned int images_s, void* filters, unsigned int filters_s, void* targets, unsigned int targets_s, int imgSize, int numFiltersPerGroup, int numGroups, dim3 grid, dim3 threads) {

  //mexPrintf("Setting up kernel\n");
  CUresult err = CUDA_SUCCESS;

  if (CUDA_SUCCESS != (err = cuFuncSetBlockShape(drvfun, threads.x, threads.y, threads.z))) {
    mexErrMsgTxt("Error in cuFuncSetBlockShape");
  }

  // add parameters
  int poffset = 0;

  if (CUDA_SUCCESS
      != cuParamSetv(drvfun, poffset, images, images_s)) {
    mexErrMsgTxt("Error in cuParamSetv");
  }
  poffset += images_s;

  if (CUDA_SUCCESS
      != cuParamSetv(drvfun, poffset, filters, filters_s)) {
    mexErrMsgTxt("Error in cuParamSetv");
  }
  poffset += filters_s;

  if (CUDA_SUCCESS
      != cuParamSetv(drvfun, poffset, targets, targets_s)) {
    mexErrMsgTxt("Error in cuParamSetv");
  }
  poffset += targets_s;

  //Next, the int imgSize
  if (CUDA_SUCCESS != cuParamSeti(drvfun, poffset, imgSize)) {
    mexErrMsgTxt("Error in cuParamSeti");
  }
  poffset += sizeof(imgSize);

  //Next, the int numFiltersPerGroup
  if (CUDA_SUCCESS != cuParamSeti(drvfun, poffset, numFiltersPerGroup)) {
    mexErrMsgTxt("Error in cuParamSeti");
  }
  poffset += sizeof(numFiltersPerGroup);

  //Next, the int numGroups
  if (CUDA_SUCCESS != cuParamSeti(drvfun, poffset, numGroups)) {
    mexErrMsgTxt("Error in cuParamSeti");
  }
  poffset += sizeof(numGroups);

  //total parameter size
  if (CUDA_SUCCESS != cuParamSetSize(drvfun, poffset)) {
    mexErrMsgTxt("Error in cuParamSetSize");
  }

  err = cuLaunchGridAsync(drvfun, grid.x, grid.y, 0);
  if (CUDA_SUCCESS != err) {
    mexErrMsgTxt("Error running kernel");
  }

}

void _conv2_bw_fit_4x16_2per(CUfunction drvfun, void* images, unsigned int images_s, void* filters, unsigned int filters_s, void* targets, unsigned int targets_s, int imgSize, dim3 grid, dim3 threads) {

  //mexPrintf("Setting up kernel\n");
  CUresult err = CUDA_SUCCESS;

  if (CUDA_SUCCESS != (err = cuFuncSetBlockShape(drvfun, threads.x, threads.y, threads.z))) {
    mexErrMsgTxt("Error in cuFuncSetBlockShape");
  }

  // add parameters
  int poffset = 0;

  if (CUDA_SUCCESS
      != cuParamSetv(drvfun, poffset, images, images_s)) {
    mexErrMsgTxt("Error in cuParamSetv");
  }
  poffset += images_s;

  if (CUDA_SUCCESS
      != cuParamSetv(drvfun, poffset, filters, filters_s)) {
    mexErrMsgTxt("Error in cuParamSetv");
  }
  poffset += filters_s;

  if (CUDA_SUCCESS
      != cuParamSetv(drvfun, poffset, targets, targets_s)) {
    mexErrMsgTxt("Error in cuParamSetv");
  }
  poffset += targets_s;

  //Next, the int imgSize
  if (CUDA_SUCCESS != cuParamSeti(drvfun, poffset, imgSize)) {
    mexErrMsgTxt("Error in cuParamSeti");
  }
  poffset += sizeof(imgSize);

  //total parameter size
  if (CUDA_SUCCESS != cuParamSetSize(drvfun, poffset)) {
    mexErrMsgTxt("Error in cuParamSetSize");
  }

  err = cuLaunchGridAsync(drvfun, grid.x, grid.y, 0);
  if (CUDA_SUCCESS != err) {
    mexErrMsgTxt("Error running kernel");
  }

}

void _conv5_bw_nofit_16x16(CUfunction drvfun, void* images, unsigned int images_s, void* filters, unsigned int filters_s, void* targets, unsigned int targets_s, int imgSize, int filterSize, int numFiltersPerGroup, int numGroups, dim3 grid, dim3 threads) {

  //mexPrintf("Setting up kernel\n");
  CUresult err = CUDA_SUCCESS;

  if (CUDA_SUCCESS != (err = cuFuncSetBlockShape(drvfun, threads.x, threads.y, threads.z))) {
    mexErrMsgTxt("Error in cuFuncSetBlockShape");
  }

  // add parameters
  int poffset = 0;


  if (CUDA_SUCCESS
      != cuParamSetv(drvfun, poffset, images, images_s)) {
    mexErrMsgTxt("Error in cuParamSetv");
  }
  poffset += images_s;

  if (CUDA_SUCCESS
      != cuParamSetv(drvfun, poffset, filters, filters_s)) {
    mexErrMsgTxt("Error in cuParamSetv");
  }
  poffset += filters_s;

  if (CUDA_SUCCESS
      != cuParamSetv(drvfun, poffset, targets, targets_s)) {
    mexErrMsgTxt("Error in cuParamSetv");
  }
  poffset += targets_s;

  //Next, the int imgSize
  if (CUDA_SUCCESS != cuParamSeti(drvfun, poffset, imgSize)) {
    mexErrMsgTxt("Error in cuParamSeti");
  }
  poffset += sizeof(imgSize);

  //Next, the int filterSize
  if (CUDA_SUCCESS != cuParamSeti(drvfun, poffset, filterSize)) {
    mexErrMsgTxt("Error in cuParamSeti");
  }
  poffset += sizeof(filterSize);

  //Next, the int numFiltersPerGroup
  if (CUDA_SUCCESS != cuParamSeti(drvfun, poffset, numFiltersPerGroup)) {
    mexErrMsgTxt("Error in cuParamSeti");
  }
  poffset += sizeof(numFiltersPerGroup);

  //Next, the int numGroups
  if (CUDA_SUCCESS != cuParamSeti(drvfun, poffset, numGroups)) {
    mexErrMsgTxt("Error in cuParamSeti");
  }
  poffset += sizeof(numGroups);


  //total parameter size
  if (CUDA_SUCCESS != cuParamSetSize(drvfun, poffset)) {
    mexErrMsgTxt("Error in cuParamSetSize");
  }

  err = cuLaunchGridAsync(drvfun, grid.x, grid.y, 0);
  if (CUDA_SUCCESS != err) {
    mexErrMsgTxt("Error running kernel");
  }

}

void _conv2_bw_nofit_dynXYZ_2per(CUfunction drvfun, void* images, unsigned int images_s, void* filters, unsigned int filters_s, void* targets, unsigned int targets_s, int imgSize, int filterSize, int numFilters, dim3 grid, dim3 threads) {

  //mexPrintf("Setting up kernel\n");
  CUresult err = CUDA_SUCCESS;

  //mexPrintf("threads.x: %d threads.y: %d threads.z: %d\n",threads.x,threads.y,threads.z);

  if (CUDA_SUCCESS != (err = cuFuncSetBlockShape(drvfun, threads.x, threads.y, threads.z))) {
    mexErrMsgTxt("Error in cuFuncSetBlockShape");
  }

  // add parameters
  int poffset = 0;


  if (CUDA_SUCCESS
      != cuParamSetv(drvfun, poffset, images, images_s)) {
    mexErrMsgTxt("Error in cuParamSetv");
  }
  poffset += images_s;

  if (CUDA_SUCCESS
      != cuParamSetv(drvfun, poffset, filters, filters_s)) {
    mexErrMsgTxt("Error in cuParamSetv");
  }
  poffset += filters_s;

  if (CUDA_SUCCESS
      != cuParamSetv(drvfun, poffset, targets, targets_s)) {
    mexErrMsgTxt("Error in cuParamSetv");
  }
  poffset += targets_s;

  //Next, the int imgSize
  if (CUDA_SUCCESS != cuParamSeti(drvfun, poffset, imgSize)) {
    mexErrMsgTxt("Error in cuParamSeti");
  }
  poffset += sizeof(imgSize);

  //Next, the int filterSize
  if (CUDA_SUCCESS != cuParamSeti(drvfun, poffset, filterSize)) {
    mexErrMsgTxt("Error in cuParamSeti");
  }
  poffset += sizeof(filterSize);

  //Next, the int numFilters
  if (CUDA_SUCCESS != cuParamSeti(drvfun, poffset, numFilters)) {
    mexErrMsgTxt("Error in cuParamSeti");
  }
  poffset += sizeof(numFilters);

  //total parameter size
  if (CUDA_SUCCESS != cuParamSetSize(drvfun, poffset)) {
    mexErrMsgTxt("Error in cuParamSetSize");
  }

  err = cuLaunchGridAsync(drvfun, grid.x, grid.y, 0);
  if (CUDA_SUCCESS != err) {
    mexErrMsgTxt("Error running kernel");
  }

}

void _convolve5_bw(void *images, unsigned int images_s, void *filters, unsigned int filters_s, void *targets, unsigned int targets_s, int numImgsPerGroup, int numFiltersPerGroup, int numGroups, int imgSize, int filterSize, int stride, ORDER imgOrder) {

  if (stride != 1 && stride != 3)
    mexErrMsgTxt("Incorrect stride; must be 1 or 3");

  //assert(imagesPerFilter == 1 || imagesPerFilter == 3);
  int numOutputsX = imgSize - filterSize + 1;
  //    int numOutputs = numOutputsX*numOutputsX;
  bool checkOutputBounds = numOutputsX % 16 != 0;



    if(filterSize > 37) {
        int numPartsX = DIVUP(numOutputsX, 16);
        int numParts = numPartsX*numPartsX;
        int blocksY = numParts*numFiltersPerGroup;
        int blocksX = numImgsPerGroup * numGroups; // Do now this is num_input_maps *num_feature_maps * num_cases = sF(3)*sF(5) like before.
        //mexPrintf("blocksX %d\n",blocksX);
        dim3 grid(blocksX, blocksY);
        dim3 threads(16, 16);
        bool checkFilterBounds = filterSize % 16 != 0;
//        printf("check filter bounds: %d, check output bounds: %d, stride: %d\n", checkFilterBounds, checkOutputBounds, stride);
        if (imgOrder == GROUP_FILTER_IMAGE) {
            if(checkFilterBounds) {
                if (checkOutputBounds) {
                    if (stride == 1) {
// 		      _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_T_T_1_2_128,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
// 		      //conv2_bw_nofit_dynXYZ_2per<true, true, 1, 2, 128><<<grid, threads>>>(images, filters, targets, imgSize, filterSize, numFilters);


                        _conv5_bw_nofit_16x16(conv5_bw_nofit_16x16_T_T_1_T,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_nofit_16x16(conv5_bw_nofit_16x16_T_T_3_T,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_nofit_16x16(conv5_bw_nofit_16x16_F_T_1_T,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_nofit_16x16(conv5_bw_nofit_16x16_F_T_3_T,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            } else {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_nofit_16x16(conv5_bw_nofit_16x16_T_F_1_T,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_nofit_16x16(conv5_bw_nofit_16x16_T_F_3_T,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_nofit_16x16(conv5_bw_nofit_16x16_F_F_1_T,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_nofit_16x16(conv5_bw_nofit_16x16_F_F_3_T,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }
        } else {
            if(checkFilterBounds) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_nofit_16x16(conv5_bw_nofit_16x16_T_T_1_F,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_nofit_16x16(conv5_bw_nofit_16x16_T_T_3_F,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_nofit_16x16(conv5_bw_nofit_16x16_F_T_1_F,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_nofit_16x16(conv5_bw_nofit_16x16_F_T_3_F,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            } else {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_nofit_16x16(conv5_bw_nofit_16x16_T_F_1_F,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_nofit_16x16(conv5_bw_nofit_16x16_T_F_3_F,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_nofit_16x16(conv5_bw_nofit_16x16_F_F_1_F,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_nofit_16x16(conv5_bw_nofit_16x16_F_F_3_F,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }
        }
    } else {
        int numPartsX = DIVUP(numOutputsX, 16);
        int numParts = numPartsX*numPartsX;
        int blocksY = numParts*numFiltersPerGroup, blocksX = numImgsPerGroup * numGroups;
        dim3 grid(blocksX, blocksY);
        dim3 threads(16, 16);
//            printf("numFiltersPerGroup: %d, numImgsPerGroup: %d, numGroups: %d\n", numFiltersPerGroup, numImgsPerGroup, numGroups);
//            printf("blocksX: %d\n", blocksX);
//            printf("stride: %d\n", stride);
        /*
         * This code was auto-generated...
         */
        if(imgOrder == GROUP_FILTER_IMAGE) {
            if (filterSize == 1) {
                throw "try multByScalar";
            } else if (filterSize == 2) {
                if (checkOutputBounds) {
                    if (stride == 1) {
		      //conv5_bw_fit_16x16<2, true, 1, true><<<grid, threads>>>(images, filters, targets, imgSize, numFiltersPerGroup, numGroups);
			_conv5_bw_fit_16x16(conv5_bw_fit_16x16_2_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_2_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_2_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_2_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 3) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_3_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_3_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_3_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_3_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 4) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_4_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_4_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_4_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_4_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 5) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_5_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_5_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_5_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_5_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 6) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_6_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_6_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_6_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_6_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 7) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_7_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_7_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_7_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_7_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 8) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_8_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_8_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_8_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_8_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 9) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_9_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_9_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_9_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_9_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 10) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_10_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_10_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_10_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_10_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 11) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_11_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_11_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_11_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_11_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 12) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_12_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_12_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_12_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_12_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 13) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_13_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_13_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_13_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_13_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 14) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_14_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_14_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_14_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_14_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 15) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_15_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_15_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_15_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_15_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 16) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_16_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_16_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_16_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_16_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 17) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_17_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_17_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_17_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_17_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 18) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_18_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_18_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_18_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_18_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 19) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_19_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_19_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_19_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_19_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 20) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_20_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_20_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_20_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_20_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 21) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_21_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_21_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_21_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_21_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 22) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_22_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_22_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_22_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_22_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 23) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_23_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_23_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_23_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_23_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 24) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_24_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_24_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_24_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_24_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 25) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_25_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_25_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_25_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_25_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 26) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_26_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_26_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_26_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_26_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 27) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_27_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_27_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_27_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_27_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 28) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_28_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_28_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_28_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_28_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 29) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_29_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_29_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_29_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_29_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 30) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_30_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_30_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_30_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_30_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 31) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_31_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_31_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_31_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_31_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 32) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_32_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_32_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_32_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_32_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 33) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_33_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_33_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_33_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_33_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 34) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_34_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_34_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_34_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_34_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 35) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_35_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_35_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_35_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_35_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 36) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_36_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_36_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_36_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_36_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 37) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_37_T_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_37_T_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_37_F_1_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_37_F_3_T, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }
        } else {
            if (filterSize == 1) {
                throw "try multByScalar";
            } else if (filterSize == 2) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_2_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_2_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_2_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_2_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 3) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_3_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_3_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_3_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_3_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 4) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_4_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_4_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_4_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_4_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 5) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_5_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_5_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_5_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_5_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 6) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_6_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_6_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_6_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_6_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 7) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_7_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_7_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_7_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_7_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 8) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_8_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_8_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_8_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_8_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 9) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_9_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_9_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_9_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_9_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 10) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_10_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_10_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_10_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_10_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 11) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_11_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_11_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_11_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_11_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 12) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_12_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_12_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_12_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_12_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 13) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_13_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_13_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_13_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_13_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 14) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_14_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_14_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_14_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_14_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 15) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_15_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_15_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_15_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_15_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 16) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_16_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_16_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_16_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_16_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 17) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_17_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_17_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_17_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_17_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 18) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_18_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_18_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_18_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_18_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 19) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_19_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_19_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_19_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_19_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 20) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_20_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_20_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_20_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_20_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 21) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_21_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_21_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_21_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_21_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 22) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_22_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_22_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_22_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_22_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 23) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_23_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_23_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_23_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_23_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 24) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_24_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_24_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_24_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_24_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 25) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_25_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_25_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_25_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_25_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 26) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_26_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_26_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_26_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_26_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 27) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_27_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_27_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_27_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_27_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 28) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_28_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_28_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_28_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_28_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 29) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_29_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_29_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_29_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_29_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 30) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_30_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_30_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_30_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_30_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 31) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_31_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_31_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_31_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_31_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 32) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_32_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_32_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_32_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_32_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 33) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_33_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_33_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_33_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_33_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 34) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_34_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_34_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_34_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_34_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 35) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_35_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_35_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_35_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_35_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 36) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_36_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_36_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_36_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_36_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }  else if (filterSize == 37) {
                if (checkOutputBounds) {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_37_T_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_37_T_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                } else {
                    if (stride == 1) {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_37_F_1_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    } else {
                        _conv5_bw_fit_16x16(conv5_bw_fit_16x16_37_F_3_F, images, images_s, filters, filters_s, targets, targets_s, imgSize, numFiltersPerGroup, numGroups, grid, threads);
                    }
                }
            }
        }
    }
    //cutilCheckMsg("kernel execution failed");



//   /* Note that Alex originally called the special routine when numOutputsX <= 8
//      However, I am having issues when numOutputsX=2
//      I get an exception when I try to issue
//      cuFuncSetBlockShape(drvfun, threads.x, threads.y, threads.z)
//      when threads.x=2, threads.y=2, threads.z=128
//      I am not sure why
//      So for now, just use the regular code to handle the numOutputsX=2 case */
//   //if (/*false  &&*/numOutputsX <= 8) {
//     if (/*false  &&*/numOutputsX <= 8 && numOutputsX>2) {
//       //mexPrintf("Special case numOutputsX: %d\n",numOutputsX);
//         /*
//          * Call special dynamic routine which is fast when the number of outputs is small.
//          */
//         int threadsX = numOutputsX, threadsY = numOutputsX, threadsZ = 512 / (threadsX*threadsY);
//         int blocksX = numCases, blocksY = DIVUP(numFilters, threadsZ*2);
//         bool checkFilterBounds = filterSize % threadsX != 0;
//         bool checkFilterIdxBounds = numFilters % (threadsZ*2) != 0;
//         dim3 grid(blocksX, blocksY);
//         dim3 threads(threadsX, threadsY, threadsZ);
//         //mexPrintf("numcases: %d, numfilters: %d, imgsize: %d, filtersize: %d\n", numCases, numFilters, imgSize, filterSize);
//         //mexPrintf("check filter bds: %d, idx bds: %d\n", checkFilterBounds, checkFilterIdxBounds);
// //        printf("grid: %dx%d\n", grid.x, grid.y);
// //        printf("threads: %dx%dx%d\n", threads.x, threads.y, threads.z);

//         if (threadsX == 2) {
//             if (checkFilterBounds) {
//                 if(checkFilterIdxBounds) {
//                     if (stride == 1) {
// 		      _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_T_T_1_2_128,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
// 		      //conv2_bw_nofit_dynXYZ_2per<true, true, 1, 2, 128><<<grid, threads>>>(images, filters, targets, imgSize, filterSize, numFilters);
//                     } else {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_T_T_3_2_128,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     }
//                 } else {
//                     if (stride == 1) {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_T_F_1_2_128,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     } else {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_T_F_3_2_128,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     }
//                 }
//             } else {
//                 if(checkFilterIdxBounds) {
//                     if (stride == 1) {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_F_T_1_2_128,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     } else {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_F_T_3_2_128,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     }
//                 } else {
//                     if (stride == 1) {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_F_F_1_2_128,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     } else {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_F_F_3_2_128,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     }
//                 }
//             }
//         } else if (threadsX == 3) {
//             if (checkFilterBounds) {
//                 if(checkFilterIdxBounds) {
//                     if (stride == 1) {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_T_T_1_3_56,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     } else {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_T_T_3_3_56,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     }
//                 } else {
//                     if (stride == 1) {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_T_F_1_3_56,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     } else {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_T_F_3_3_56,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     }
//                 }
//             } else {
//                 if(checkFilterIdxBounds) {
//                     if (stride == 1) {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_F_T_1_3_56,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     } else {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_F_T_3_3_56,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     }
//                 } else {
//                     if (stride == 1) {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_F_F_1_3_56,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     } else {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_F_F_3_3_56,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     }
//                 }
//             }
//         }  else if (threadsX == 4) {
//             if (checkFilterBounds) {
//                 if(checkFilterIdxBounds) {
//                     if (stride == 1) {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_T_T_1_4_32,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     } else {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_T_T_3_4_32,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     }
//                 } else {
//                     if (stride == 1) {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_T_F_1_4_32,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     } else {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_T_F_3_4_32,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     }
//                 }
//             } else {
//                 if(checkFilterIdxBounds) {
//                     if (stride == 1) {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_F_T_1_4_32,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     } else {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_F_T_3_4_32,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     }
//                 } else {
//                     if (stride == 1) {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_F_F_1_4_32,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     } else {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_F_F_3_4_32,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     }
//                 }
//             }
//         }  else if (threadsX == 5) {
//             if (checkFilterBounds) {
//                 if(checkFilterIdxBounds) {
//                     if (stride == 1) {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_T_T_1_5_20,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     } else {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_T_T_3_5_20,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     }
//                 } else {
//                     if (stride == 1) {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_T_F_1_5_20,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     } else {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_T_F_3_5_20,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     }
//                 }
//             } else {
//                 if(checkFilterIdxBounds) {
//                     if (stride == 1) {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_F_T_1_5_20,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     } else {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_F_T_3_5_20,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     }
//                 } else {
//                     if (stride == 1) {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_F_F_1_5_20,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     } else {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_F_F_3_5_20,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     }
//                 }
//             }
//         }  else if (threadsX == 6) {
//             if (checkFilterBounds) {
//                 if(checkFilterIdxBounds) {
//                     if (stride == 1) {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_T_T_1_6_14,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     } else {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_T_T_3_6_14,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     }
//                 } else {
//                     if (stride == 1) {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_T_F_1_6_14,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     } else {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_T_F_3_6_14,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     }
//                 }
//             } else {
//                 if(checkFilterIdxBounds) {
//                     if (stride == 1) {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_F_T_1_6_14,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     } else {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_F_T_3_6_14,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     }
//                 } else {
//                     if (stride == 1) {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_F_F_1_6_14,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     } else {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_F_F_3_6_14,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     }
//                 }
//             }
//         }  else if (threadsX == 7) {
//             if (checkFilterBounds) {
//                 if(checkFilterIdxBounds) {
//                     if (stride == 1) {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_T_T_1_7_10,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     } else {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_T_T_3_7_10,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     }
//                 } else {
//                     if (stride == 1) {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_T_F_1_7_10,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     } else {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_T_F_3_7_10,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     }
//                 }
//             } else {
//                 if(checkFilterIdxBounds) {
//                     if (stride == 1) {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_F_T_1_7_10,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     } else {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_F_T_3_7_10,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     }
//                 } else {
//                     if (stride == 1) {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_F_F_1_7_10,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     } else {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_F_F_3_7_10,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     }
//                 }
//             }
//         }  else if (threadsX == 8) {
//             if (checkFilterBounds) {
//                 if(checkFilterIdxBounds) {
//                     if (stride == 1) {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_T_T_1_8_8,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     } else {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_T_T_3_8_8,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     }
//                 } else {
//                     if (stride == 1) {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_T_F_1_8_8,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     } else {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_T_F_3_8_8,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     }
//                 }
//             } else {
//                 if(checkFilterIdxBounds) {
//                     if (stride == 1) {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_F_T_1_8_8,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     } else {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_F_T_3_8_8,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     }
//                 } else {
//                     if (stride == 1) {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_F_F_1_8_8,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     } else {
//                         _conv2_bw_nofit_dynXYZ_2per(conv2_bw_nofit_dynXYZ_2per_F_F_3_8_8,images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, numFilters, grid, threads);
//                     }
//                 }
//             }
//         }
//     } else if(filterSize > 20) {
//     bool checkFilterBounds = filterSize % 16 != 0;
//     int blocksY = numFilters / 16, blocksX = numCases;
//     dim3 grid(blocksX, blocksY);
//     dim3 threads(16, 4, 8);

//     if(checkFilterBounds) {
//       if(stride == 1) {
// 	_conv2_bw_nofit_4x16_2per(conv2_bw_nofit_4x16_2per_11, images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, grid, threads);

//       } else {
// 	_conv2_bw_nofit_4x16_2per(conv2_bw_nofit_4x16_2per_13, images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, grid, threads);
//       } 
//     } 
//     else {
//       if(stride ==1) {
// 	_conv2_bw_nofit_4x16_2per(conv2_bw_nofit_4x16_2per_01, images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, grid, threads);
//       } else{
// 	_conv2_bw_nofit_4x16_2per(conv2_bw_nofit_4x16_2per_03, images, images_s, filters, filters_s, targets, targets_s, imgSize, filterSize, grid, threads);
//       }
//     }
//   } else if(filterSize > 14) {

//     int blocksY = numFilters / 8, blocksX = numCases;
//     dim3 grid(blocksX, blocksY);
//     dim3 threads(16, 4, 8);

//     /*
//     mexPrintf("grid.x: %d\n",grid.x);
//     mexPrintf("grid.y: %d\n",grid.y);
//     mexPrintf("threads.x: %d\n",threads.x);
//     mexPrintf("threads.y: %d\n",threads.y);
//     mexPrintf("threads.z: %d\n",threads.z);
//     */

//     if(filterSize == 15) {
//       if(checkOutputBounds) {
// 	if (stride == 1) {
// 	  _conv2_bw_fit_4x16_1per(conv2_bw_fit_4x16_1per_1511, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	} else {
// 	  _conv2_bw_fit_4x16_1per(conv2_bw_fit_4x16_1per_1513, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	}
//       } else {
// 	if (stride == 1) {
// 	  _conv2_bw_fit_4x16_1per(conv2_bw_fit_4x16_1per_1501, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	} else {
// 	  _conv2_bw_fit_4x16_1per(conv2_bw_fit_4x16_1per_1503, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	}
//       }
//     } else if(filterSize == 16) {
//       if(checkOutputBounds) {
// 	if (stride == 1) {
// 	  _conv2_bw_fit_4x16_1per(conv2_bw_fit_4x16_1per_1611, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	} else {
// 	  _conv2_bw_fit_4x16_1per(conv2_bw_fit_4x16_1per_1613, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	}
//       } else {
// 	if (stride == 1) {
// 	  _conv2_bw_fit_4x16_1per(conv2_bw_fit_4x16_1per_1601, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	} else {
// 	  _conv2_bw_fit_4x16_1per(conv2_bw_fit_4x16_1per_1603, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	}
//       }
//     }
//     else if(filterSize == 17) {
//       if(checkOutputBounds) {
// 	if (stride == 1) {
// 	  _conv2_bw_fit_4x16_1per(conv2_bw_fit_4x16_1per_1711, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	} else {
// 	  _conv2_bw_fit_4x16_1per(conv2_bw_fit_4x16_1per_1713, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	}
//       } else {
// 	if (stride == 1) {
// 	  _conv2_bw_fit_4x16_1per(conv2_bw_fit_4x16_1per_1701, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	} else {
// 	  _conv2_bw_fit_4x16_1per(conv2_bw_fit_4x16_1per_1703, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	}
//       }
//     }
//     else if(filterSize == 18) {
//       if(checkOutputBounds) {
// 	if (stride == 1) {
// 	  _conv2_bw_fit_4x16_1per(conv2_bw_fit_4x16_1per_1811, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	} else {
// 	  _conv2_bw_fit_4x16_1per(conv2_bw_fit_4x16_1per_1813, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	}
//       } else {
// 	if (stride == 1) {
// 	  _conv2_bw_fit_4x16_1per(conv2_bw_fit_4x16_1per_1801, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	} else {
// 	  _conv2_bw_fit_4x16_1per(conv2_bw_fit_4x16_1per_1803, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	}
//       }
	  
//     }
//     else if(filterSize == 19) {
//       if(checkOutputBounds) {
// 	if (stride == 1) {
// 	  _conv2_bw_fit_4x16_1per(conv2_bw_fit_4x16_1per_1911, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	} else {
// 	  _conv2_bw_fit_4x16_1per(conv2_bw_fit_4x16_1per_1913, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	}
//       } else {
// 	if (stride == 1) {
// 	  _conv2_bw_fit_4x16_1per(conv2_bw_fit_4x16_1per_1901, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	} else {
// 	  _conv2_bw_fit_4x16_1per(conv2_bw_fit_4x16_1per_1903, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	}
//       }
//     }
//     else if(filterSize == 20) {
//       if(checkOutputBounds) {
// 	if (stride == 1) {
// 	  _conv2_bw_fit_4x16_1per(conv2_bw_fit_4x16_1per_2011, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	} else {
// 	  _conv2_bw_fit_4x16_1per(conv2_bw_fit_4x16_1per_2013, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	}
//       } else {
// 	if (stride == 1) {
// 	  _conv2_bw_fit_4x16_1per(conv2_bw_fit_4x16_1per_2001, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	} else {
// 	  _conv2_bw_fit_4x16_1per(conv2_bw_fit_4x16_1per_2003, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	}
//       }
//     } 
//   }  else {
//       int blocksY = numFilters / 16, blocksX = numCases;
//       dim3 grid(blocksX, blocksY);
//       dim3 threads(16, 4, 8);
	
//       if(filterSize == 2) {
// 	if(checkOutputBounds) {
// 	  if (stride == 1) {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_211, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  } else {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_213, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  }
// 	} else {
// 	  if (stride == 1) {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_201, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  } else {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_203, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  }
// 	}
//       } else if(filterSize == 3) {
// 	if(checkOutputBounds) {
// 	  if (stride == 1) {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_311, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  } else {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_313, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  }
// 	} else {
// 	  if (stride == 1) {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_301, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  } else {
	    
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_303, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  }
// 	}
//       }else if(filterSize == 4) {
// 	if(checkOutputBounds) {
// 	  if (stride == 1) {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_411, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  } else {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_413, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  }
// 	} else {
// 	  if (stride == 1) {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_401, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  } else {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_403, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  }
// 	}
//       }else if(filterSize == 5) {
// 	if(checkOutputBounds) {
// 	  if (stride == 1) {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_511, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  } else {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_513, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  }
// 	} else {
// 	  if (stride == 1) {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_501, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  } else {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_503, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  }
// 	}
//       }else if(filterSize == 6) {
// 	if(checkOutputBounds) {
// 	  if (stride == 1) {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_611, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  } else {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_613, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  }
// 	} else {
// 	  if (stride == 1) {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_601, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  } else {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_603, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  }
// 	}
//       }else if(filterSize == 7) {
// 	if(checkOutputBounds) {
// 	  if (stride == 1) {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_711, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  } else {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_713, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  }
// 	} else {
// 	  if (stride == 1) {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_701, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  } else {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_703, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  }
// 	}
//       }else if(filterSize == 8) {
// 	if(checkOutputBounds) {
// 	  if (stride == 1) {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_811, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  } else {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_813, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  }
// 	} else {
// 	  if (stride == 1) {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_801, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  } else {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_803, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  }
// 	}
//       }else if(filterSize == 9) {
// 	if(checkOutputBounds) {
// 	  if (stride == 1) {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_911, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  } else {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_913, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  }
// 	} else {
// 	  if (stride == 1) {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_901, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  } else {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_903, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  }
// 	}
//       }else if(filterSize == 10) {
// 	if(checkOutputBounds) {
// 	  if (stride == 1) {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_1011, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  } else {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_1013, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  }
// 	} else {
// 	  if (stride == 1) {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_1001, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  } else {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_1003, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  }
// 	}
//       }else if(filterSize == 11) {
// 	if(checkOutputBounds) {
// 	  if (stride == 1) {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_1111, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  } else {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_1113, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  }
// 	} else {
// 	  if (stride == 1) {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_1101, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  } else {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_1103, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  }
// 	}
//       }else if(filterSize == 12) {
// 	if(checkOutputBounds) {
// 	  if (stride == 1) {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_1211, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  } else {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_1213, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  }
// 	} else {
// 	  if (stride == 1) {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_1201, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  } else {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_1203, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  }
// 	}
//       } else if(filterSize == 13) {
// 	if(checkOutputBounds) {
// 	  if (stride == 1) {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_1311, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  } else {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_1313, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  }
// 	} else {
// 	  if (stride == 1) {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_1301, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  } else {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_1303, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  }
// 	}
//       } else if(filterSize == 14) {
// 	if(checkOutputBounds) {
// 	  if (stride == 1) {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_1411, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  } else {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_1413, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  }
// 	} else {
// 	  if (stride == 1) {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_1401, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  } else {
// 	    _conv2_bw_fit_4x16_2per(conv2_bw_fit_4x16_2per_1403, images, images_s, filters, filters_s, targets, targets_s, imgSize, grid, threads);
// 	  }
// 	}
//       }
//     }
//     //cutilCheckMsg("kernel execution failed");
} 

/* Mimics convolve5 in conv5.cu */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {

  CUresult cudastatus = CUDA_SUCCESS;

  if (nrhs != 6)
    //Should pass in images, filters, targets, numGroups, color, imgOrder
    mexErrMsgTxt("Wrong number of arguments");

  if (init == 0) {
    // Initialize function
//     mexLock();

    // load GPUmat
    gm = gmGetGPUmat();

    /* Set up modules here, so they only need to be loaded the first time the function is called */
    CUmodule *drvmod = gmGetModule("conv5");

    //load appropriate GPU kernel (mangled name)
    CUresult status;
    status = cuModuleGetFunction(&conv5_bw_nofit_16x16_F_F_1_F, *drvmod, "_Z20conv5_bw_nofit_16x16ILb0ELb0ELi1ELb0EEvPfS0_S0_iiii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_nofit_16x16_F_F_1_T, *drvmod, "_Z20conv5_bw_nofit_16x16ILb0ELb0ELi1ELb1EEvPfS0_S0_iiii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_nofit_16x16_F_F_3_F, *drvmod, "_Z20conv5_bw_nofit_16x16ILb0ELb0ELi3ELb0EEvPfS0_S0_iiii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_nofit_16x16_F_F_3_T, *drvmod, "_Z20conv5_bw_nofit_16x16ILb0ELb0ELi3ELb1EEvPfS0_S0_iiii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_nofit_16x16_F_T_1_F, *drvmod, "_Z20conv5_bw_nofit_16x16ILb0ELb1ELi1ELb0EEvPfS0_S0_iiii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_nofit_16x16_F_T_1_T, *drvmod, "_Z20conv5_bw_nofit_16x16ILb0ELb1ELi1ELb1EEvPfS0_S0_iiii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_nofit_16x16_F_T_3_F, *drvmod, "_Z20conv5_bw_nofit_16x16ILb0ELb1ELi3ELb0EEvPfS0_S0_iiii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_nofit_16x16_F_T_3_T, *drvmod, "_Z20conv5_bw_nofit_16x16ILb0ELb1ELi3ELb1EEvPfS0_S0_iiii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_nofit_16x16_T_F_1_F, *drvmod, "_Z20conv5_bw_nofit_16x16ILb1ELb0ELi1ELb0EEvPfS0_S0_iiii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_nofit_16x16_T_F_1_T, *drvmod, "_Z20conv5_bw_nofit_16x16ILb1ELb0ELi1ELb1EEvPfS0_S0_iiii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_nofit_16x16_T_F_3_F, *drvmod, "_Z20conv5_bw_nofit_16x16ILb1ELb0ELi3ELb0EEvPfS0_S0_iiii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_nofit_16x16_T_F_3_T, *drvmod, "_Z20conv5_bw_nofit_16x16ILb1ELb0ELi3ELb1EEvPfS0_S0_iiii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_nofit_16x16_T_T_1_F, *drvmod, "_Z20conv5_bw_nofit_16x16ILb1ELb1ELi1ELb0EEvPfS0_S0_iiii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_nofit_16x16_T_T_1_T, *drvmod, "_Z20conv5_bw_nofit_16x16ILb1ELb1ELi1ELb1EEvPfS0_S0_iiii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_nofit_16x16_T_T_3_F, *drvmod, "_Z20conv5_bw_nofit_16x16ILb1ELb1ELi3ELb0EEvPfS0_S0_iiii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_nofit_16x16_T_T_3_T, *drvmod, "_Z20conv5_bw_nofit_16x16ILb1ELb1ELi3ELb1EEvPfS0_S0_iiii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_2_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi2ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_2_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi2ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_2_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi2ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_2_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi2ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_2_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi2ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_2_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi2ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_2_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi2ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_2_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi2ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_3_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi3ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_3_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi3ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_3_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi3ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_3_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi3ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_3_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi3ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_3_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi3ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_3_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi3ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_3_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi3ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_4_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi4ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_4_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi4ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_4_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi4ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_4_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi4ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_4_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi4ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_4_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi4ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_4_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi4ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_4_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi4ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_5_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi5ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_5_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi5ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_5_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi5ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_5_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi5ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_5_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi5ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_5_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi5ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_5_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi5ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_5_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi5ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_6_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi6ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_6_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi6ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_6_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi6ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_6_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi6ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_6_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi6ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_6_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi6ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_6_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi6ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_6_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi6ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_7_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi7ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_7_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi7ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_7_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi7ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_7_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi7ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_7_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi7ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_7_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi7ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_7_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi7ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_7_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi7ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_8_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi8ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_8_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi8ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_8_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi8ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_8_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi8ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_8_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi8ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_8_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi8ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_8_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi8ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_8_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi8ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_9_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi9ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_9_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi9ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_9_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi9ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_9_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi9ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_9_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi9ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_9_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi9ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_9_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi9ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_9_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi9ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_10_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi10ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_10_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi10ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_10_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi10ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_10_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi10ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_10_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi10ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_10_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi10ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_10_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi10ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_10_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi10ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_11_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi11ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_11_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi11ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_11_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi11ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_11_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi11ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_11_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi11ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_11_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi11ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_11_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi11ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_11_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi11ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_12_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi12ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_12_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi12ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_12_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi12ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_12_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi12ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_12_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi12ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_12_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi12ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_12_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi12ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_12_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi12ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_13_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi13ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_13_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi13ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_13_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi13ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_13_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi13ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_13_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi13ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_13_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi13ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_13_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi13ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_13_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi13ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_14_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi14ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_14_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi14ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_14_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi14ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_14_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi14ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_14_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi14ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_14_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi14ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_14_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi14ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_14_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi14ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_15_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi15ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_15_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi15ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_15_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi15ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_15_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi15ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_15_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi15ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_15_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi15ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_15_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi15ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_15_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi15ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_16_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi16ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_16_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi16ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_16_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi16ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_16_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi16ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_16_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi16ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_16_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi16ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_16_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi16ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_16_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi16ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_17_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi17ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_17_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi17ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_17_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi17ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_17_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi17ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_17_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi17ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_17_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi17ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_17_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi17ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_17_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi17ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_18_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi18ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_18_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi18ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_18_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi18ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_18_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi18ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_18_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi18ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_18_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi18ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_18_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi18ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_18_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi18ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_19_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi19ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_19_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi19ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_19_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi19ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_19_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi19ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_19_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi19ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_19_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi19ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_19_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi19ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_19_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi19ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_20_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi20ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_20_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi20ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_20_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi20ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_20_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi20ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_20_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi20ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_20_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi20ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_20_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi20ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_20_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi20ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_21_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi21ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_21_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi21ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_21_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi21ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_21_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi21ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_21_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi21ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_21_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi21ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_21_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi21ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_21_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi21ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_22_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi22ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_22_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi22ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_22_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi22ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_22_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi22ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_22_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi22ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_22_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi22ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_22_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi22ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_22_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi22ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_23_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi23ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_23_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi23ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_23_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi23ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_23_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi23ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_23_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi23ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_23_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi23ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_23_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi23ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_23_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi23ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_24_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi24ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_24_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi24ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_24_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi24ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_24_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi24ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_24_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi24ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_24_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi24ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_24_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi24ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_24_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi24ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_25_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi25ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_25_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi25ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_25_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi25ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_25_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi25ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_25_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi25ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_25_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi25ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_25_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi25ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_25_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi25ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_26_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi26ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_26_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi26ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_26_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi26ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_26_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi26ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_26_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi26ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_26_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi26ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_26_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi26ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_26_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi26ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_27_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi27ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_27_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi27ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_27_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi27ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_27_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi27ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_27_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi27ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_27_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi27ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_27_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi27ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_27_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi27ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_28_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi28ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_28_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi28ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_28_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi28ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_28_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi28ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_28_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi28ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_28_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi28ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_28_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi28ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_28_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi28ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_29_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi29ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_29_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi29ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_29_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi29ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_29_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi29ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_29_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi29ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_29_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi29ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_29_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi29ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_29_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi29ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_30_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi30ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_30_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi30ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_30_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi30ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_30_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi30ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_30_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi30ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_30_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi30ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_30_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi30ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_30_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi30ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_31_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi31ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_31_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi31ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_31_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi31ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_31_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi31ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_31_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi31ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_31_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi31ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_31_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi31ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_31_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi31ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_32_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi32ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_32_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi32ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_32_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi32ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_32_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi32ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_32_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi32ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_32_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi32ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_32_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi32ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_32_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi32ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_33_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi33ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_33_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi33ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_33_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi33ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_33_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi33ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_33_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi33ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_33_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi33ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_33_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi33ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_33_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi33ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_34_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi34ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_34_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi34ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_34_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi34ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_34_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi34ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_34_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi34ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_34_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi34ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_34_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi34ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_34_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi34ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_35_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi35ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_35_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi35ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_35_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi35ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_35_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi35ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_35_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi35ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_35_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi35ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_35_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi35ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_35_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi35ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_36_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi36ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_36_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi36ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_36_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi36ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_36_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi36ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_36_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi36ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_36_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi36ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_36_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi36ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_36_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi36ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }

    status = cuModuleGetFunction(&conv5_bw_fit_16x16_37_F_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi37ELb0ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_37_F_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi37ELb0ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_37_F_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi37ELb0ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_37_F_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi37ELb0ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_37_T_1_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi37ELb1ELi1ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_37_T_1_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi37ELb1ELi1ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_37_T_3_F, *drvmod, "_Z18conv5_bw_fit_16x16ILi37ELb1ELi3ELb0EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }
    status = cuModuleGetFunction(&conv5_bw_fit_16x16_37_T_3_T, *drvmod, "_Z18conv5_bw_fit_16x16ILi37ELb1ELi3ELb1EEvPfS0_S0_iii");
    if (CUDA_SUCCESS != status) {
      mexErrMsgTxt("Unable to load user function.");
    }


    init = 1;
  }

  // GPUmat parameters are:

  // 1. images
  // 2. filters
  // 3. targets
  // 4. numGroups
  // 5. color
  // 6. imgOrder

  //IN1 is the input GPU array
  GPUtype IN1 = gm->gputype.getGPUtype(prhs[0]);

  //IN2 is the input GPU array
  GPUtype IN2 = gm->gputype.getGPUtype(prhs[1]);

  //OUT is the output GPU array (result)
  GPUtype OUT = gm->gputype.getGPUtype(prhs[2]);

  //fourth parameter is the numGroups (int)
  int numGroups = (int) mxGetScalar(prhs[3]);

  //fifth parameter is the color (int [0,1])
  bool color = (bool) mxGetScalar(prhs[4]);

  //sixth parameter is the order (int [0,1])
  ORDER imgOrder = (ORDER) mxGetScalar(prhs[5]);

  // number of elements
  int nin1 = gm->gputype.getNumel(IN1);
  int nin2 = gm->gputype.getNumel(IN2);
  int nout = gm->gputype.getNumel(OUT);

  //dimensions
  const int * sin1 = gm->gputype.getSize(IN1); //images
  const int * sin2 = gm->gputype.getSize(IN2); //filters
  const int * sout = gm->gputype.getSize(OUT); //targets


    // Get the number of filters per group based on F's second dimension divided by numGroups = sF(3)*sF(5)
  // This is sF(4);
  int numFiltersPerGroup = sin2[1] / numGroups; // This is the number of input_maps.
//   int numImgsPerGroup = sout[1] / numGroups; // Set to 1 in my setup.CG
  int numImgsPerGroup = sin1[1] / numGroups; // This is the number of feature_maps.
//   mexPrintf("numImgsPerGroup: %d\n",numImgsPerGroup);
//   int numCases = numGroups/numImgsPerGroup; // This is really the number of images to pass through. sF(5)
  
//   if (sout[1] % numCases != 0)
//     mexErrMsgTxt("Number of target columns not a multiple of numCases");

  int colorMult = color ? 3 : 1;

  if (sin2[0] % colorMult != 0)
    mexErrMsgTxt("Number of filter rows not a multiple of colorMult");

  if (sin1[1] % numGroups != 0)
    mexErrMsgTxt("Number of image columns not a multiple of numGroups");
  
  if (sin2[1] % numGroups != 0)
    mexErrMsgTxt("Number of filter columns not a multiple of numGroups");



  // When passing in 0 for order
  if (imgOrder == GROUP_FILTER_IMAGE) {
      if (sout[1] % numImgsPerGroup != 0)
        mexErrMsgTxt("Number of output rows not a multiple of numImgsPerGroup (ie. num_feature_maps)");

      if (sout[1] % numFiltersPerGroup != 0)
        mexErrMsgTxt("Number of output rows not a multiple of numFiltersPerGroup (ie. num_input_maps)");

      if (sout[1] % numGroups != 0)
        mexErrMsgTxt("Number of output rows not a multiple of numGroups (ie. num_cases in batch)");
  } else { // 1 for order
      mexErrMsgTxt("Only GROUP_FILTER_IMAGE type is supported in cuConv5");
      if (sin1[1] != numImgsPerGroup * numGroups)
	mexErrMsgTxt("Number of image cols must be numImgsPerGroup * numGroups");

      if (sin1[0] % numFiltersPerGroup != 0)
	mexErrMsgTxt("Number of image rows not a multiple of numFiltersPerGroup");
  }

//   int imgPixels = imgOrder == GROUP_FILTER_IMAGE ? sin1[0] / numImgsPerGroup : sin1[0] / numFiltersPerGroup;
  // Modified this so maps just need to be numPixels in the first dimension (just like the filters). 
  int imgPixels = imgOrder == GROUP_FILTER_IMAGE ? sin1[0] : sin1[0] / numFiltersPerGroup;
  int filterPixels = sin2[0] / colorMult;
  
  
  //double dImgSize = sqrt(sin1[1]);
  //double dFilterSize = sqrt(sin2[1]);

  if ( sqrt(double(imgPixels)) != floor(sqrt(double(imgPixels))))
    mexErrMsgTxt("Images are not square");

  if ( sqrt(double(filterPixels)) != floor(sqrt(double(filterPixels))))
    mexErrMsgTxt("Filters are not square");

  int imgSize = int(sqrt(double(imgPixels)));
  int filterSize = int(sqrt(double(filterPixels)));
  
  int numOutputsX = imgSize - filterSize + 1;
  int numOutputs = numOutputsX * numOutputsX;

  if (sout[0] != numOutputs * colorMult )
    mexErrMsgTxt("Number of target rows must be numOutputs * colorMult * numImgsPerGroup");

  // Now from this point on, define numGroups as the number of cases sF(5)=smaps(4)
//   numGroups = numCases;
  
  //some checks
  // if (numFilters % 16 != 0)
  //   mexErrMsgTxt("Number of filters must be a multiple of 16");
  //assert(nout == numOutputs * numFilters * numCases);
  // if (imgSize <= filterSize)
  //   mexErrMsgTxt("imgSize must > filterSize");

//   mexPrintf("imgSize: %d\n",imgSize);
//   mexPrintf("filterSize: %d\n",filterSize);
//   mexPrintf("numGroups: %d\n",numGroups);
//   mexPrintf("numImgsPerGroup: %d\n",numImgsPerGroup);
//   mexPrintf("numFiltersPerGroup: %d\n",numFiltersPerGroup);
//   mexPrintf("imgOrder: %d\n",imgOrder);
//   mexPrintf("colorMult: %d\n",colorMult);
//   mexPrintf("numOutputsX: %d\n",numOutputsX);
//   mexPrintf("numOutputs: %d\n",numOutputs);
  
//     mexErrMsgTxt("Stoping here\n");


  gpuTYPE_t tin1 = gm->gputype.getType(IN1);
  gpuTYPE_t tin2 = gm->gputype.getType(IN2);
  gpuTYPE_t tout = gm->gputype.getType(OUT);
  
  // check input/out types
  if (tin1 != gpuFLOAT ) 
    mexErrMsgTxt("Currently only gpuFLOAT type supported.");

  if (tin1!=tin2)
    mexErrMsgTxt("Input arguments must be of the same type.");

  if (tin1!=tout)
    mexErrMsgTxt("Input and output arguments must be of the same type.");

  // I need the pointers to GPU memory
  CUdeviceptr d_IN1  = (CUdeviceptr) (UINTPTR gm->gputype.getGPUptr(IN1));
  CUdeviceptr d_IN2  = (CUdeviceptr) (UINTPTR gm->gputype.getGPUptr(IN2));
  CUdeviceptr d_OUT = (CUdeviceptr) (UINTPTR gm->gputype.getGPUptr(OUT));

  //last argument is stride (1 for bw images)
  _convolve5_bw(&d_IN1, sizeof(d_IN1), &d_IN2, sizeof(d_IN2), &d_OUT, sizeof(d_OUT), numImgsPerGroup * colorMult, numFiltersPerGroup, numGroups, imgSize, filterSize, colorMult, imgOrder);  

}
