#include "GPUkernel.hh"

#define MUL24 __mul24

//To use templates we must have C++ kernels
//extern "C" {

/*
 * This version uses block size (z, y, x) = 8x4x16.
 *
 * Each block convolves 1 image with 16 filters.
 * Works for filters 14x14 or smaller; image size only influences checkBounds.
 * The checked version uses 20 registers...would be nice to get that down to 16.
 */
template<int filterSize, bool checkBounds, int imagesPerFilter>
__global__ void conv2_bw_fit_4x16_2per(float* imgs, float* filters, float* targets, int imgSize) {
    const int shImgSizeX = filterSize + 15, shImgSizeY = filterSize + 3;
    __shared__ float shImg[shImgSizeY][shImgSizeX];
    __shared__ float shFilter[16][filterSize][filterSize];

    const int imgIdx = blockIdx.x;
    const int filtIdx = 2 * 8 * blockIdx.y + 2 * threadIdx.z;
    const int numFilters = 2 * 8 * gridDim.y;
    const int pidx = threadIdx.y * 16 + threadIdx.x; // thread's index within the 4x16 "plate" of threads
    const int tidx = threadIdx.z * 4 * 16 + pidx; // thread's index within its block
    const int numOutputsX = imgSize - filterSize + 1;
    const int numOutputs = MUL24(numOutputsX, numOutputsX);
    const int imgPixels = MUL24(imgSize, imgSize); // size of image

    const int shImgPixels = shImgSizeX * shImgSizeY; // size of shared buffer for image
    const int filterPixels = filterSize * filterSize;
    const int loadX = tidx % (shImgSizeX);
    const int loadY = tidx / (shImgSizeX);

    imgs += imgIdx * imgPixels + MUL24(loadY, imgSize) + loadX;
    filters += (imgIdx / imagesPerFilter) * numFilters * filterPixels + MUL24(filtIdx, filterPixels) + pidx;
    targets += imgIdx * numFilters * numOutputs + MUL24(filtIdx, numOutputs) + MUL24(threadIdx.y, numOutputsX) + threadIdx.x;

    float* shFilterLoad = &shFilter[threadIdx.z * 2][0][pidx];
//    float* shFilterLoad2 = &shFilter[threadIdx.z * 2 + 1][0][pidx];
    float* shImgLoad = &shImg[loadY][loadX];

    for (int i = pidx; i < filterPixels; i += 16 * 4) { // Load the filter
        shFilterLoad[0] = filters[0];
        shFilterLoad[filterPixels] = filters[filterPixels];
        filters += 16 * 4;
        shFilterLoad += 16 * 4;
    }

    for(int y = 0; y < numOutputsX; y += 4) {
        for(int x = 0; x < numOutputsX; x += 16) {
            __syncthreads();
            /*
             * This will load the entire (shared) image AS LONG AS THE FILTER SIZE IS <= 14.
             * If the filter size gets too big there won't be enough threads in the block
             * to load the entire shared image memory in one go. But that's ok because
             * when the filter size is > 14, this function doesn't have enough
             * memory to run anyway.
             *
             * O = (I - F + 1)
             * If O = 16K, then I = 15 + F + 16(K - 1). This is why !checkBounds is here.
             */
            if (tidx < shImgPixels) {
                if (!checkBounds || (x + loadX < imgSize && y + loadY < imgSize)) { // very very cheap test (~0.3% of runtime)
//                    shImgLoad[0] = imgs[x];
                    shImgLoad[0] = imgs[MUL24(imgSize, y) + x];
                }
            }

            __syncthreads();

            if (!checkBounds || (x + threadIdx.x < numOutputsX && y + threadIdx.y < numOutputsX)) {
                float* myShFilter = &shFilter[2 * threadIdx.z][0][0];
                float* myShImg = &shImg[threadIdx.y][threadIdx.x];
                float prod[2] = { 0, 0 };
                // The checkBounds version sees a slight speedup here
                // from #parama unroll, but the nocheck version sees a slowdown (because occupancy goes down).
                for (int i = 0; i < filterSize; i++) {
                    for (int j = 0; j < filterSize; j++) {
                        prod[0] += myShFilter[0] * myShImg[0];
                        prod[1] += myShFilter[filterPixels] * myShImg[0];

                        myShFilter++;
                        myShImg++;
                    }
                    myShImg += 15;
                }
//                targets[0] = prod[0];
//                targets[numOutputs] = prod[1];
                targets[MUL24(y, numOutputsX) + x] = prod[0];
                targets[MUL24(y, numOutputsX) + x + numOutputs] = prod[1];
            }
//            targets += !checkBounds || x < numOutputsX - 16 ? 16 : numOutputsX - x;
        }
//        targets += MUL24(3, numOutputsX);
//        imgs += imgSize * 4;
    }
}

/*
 * This version uses block size (z, y, x).
 *
 * Each block convolves 1 image with 8 filters.
 * Works for filters 20x20 or smaller; image size only influences checkBounds.
 */
template<int filterSize, bool checkBounds, int imagesPerFilter>
__global__ void conv2_bw_fit_4x16_1per(float* imgs, float* filters, float* targets, int imgSize) {
    const int shImgSizeX = filterSize + 15, shImgSizeY = filterSize + 3;
    const int shImgPixels = shImgSizeX * shImgSizeY; // size of shared buffer for image
    __shared__ float shImg[shImgSizeY][shImgSizeX];
    __shared__ float shFilter[8][filterSize][filterSize];

    const int imgIdx = blockIdx.x;
    const int filtIdx = 8 * blockIdx.y + threadIdx.z;
    const int numFilters = 8 * gridDim.y;
    const int pidx = threadIdx.y * 16 + threadIdx.x; // thread's index within the 4x16 "plate" of threads
    const int tidx = threadIdx.z * 4 * 16 + pidx; // thread's index within its block
    const int numOutputsX = imgSize - filterSize + 1;
    const int numOutputs = MUL24(numOutputsX, numOutputsX);
    const int imgPixels = MUL24(imgSize, imgSize); // size of image
    const int filterPixels = filterSize * filterSize;

    imgs += MUL24(imgPixels, imgIdx);
    filters += (imgIdx / imagesPerFilter) * numFilters * filterPixels + MUL24(filtIdx, filterPixels) + pidx;
    targets += imgIdx * numFilters * numOutputs + MUL24(filtIdx, numOutputs) + MUL24(threadIdx.y, numOutputsX) + threadIdx.x;

    float* shFilterLoad = &shFilter[threadIdx.z][0][pidx];

    for (int i = pidx; i < filterPixels; i += 16 * 4) { // Load the filter
        shFilterLoad[0] = filters[0];
        filters += 16 * 4;
        shFilterLoad += 16 * 4;
    }

    for(int y = 0; y < numOutputsX; y += 4) {
        for(int x = 0; x < numOutputsX; x += 16) {
            __syncthreads();
            for (int i = tidx; i < shImgPixels; i += 16 * 4 * 8) {
                const int loadX = i % (shImgSizeX);
                const int loadY = i / (shImgSizeX);
                if (!checkBounds || (x + loadX < imgSize && y + loadY < imgSize)) {
                    shImg[0][i] = imgs[(loadY + y) * imgSize + loadX + x];
                }
            }

            __syncthreads();

            if (!checkBounds || (x + threadIdx.x < numOutputsX && y + threadIdx.y < numOutputsX)) {
                float* myShFilter = &shFilter[threadIdx.z][0][0];
                float* myShImg = &shImg[threadIdx.y][threadIdx.x];
                float prod = 0;

                #pragma unroll
                for (int i = 0; i < filterSize; i++) {
                    for (int j = 0; j < filterSize; j++) {
                        prod += myShFilter[0] * myShImg[0];

                        myShFilter++;
                        myShImg++;
                    }
                    myShImg += 15;
                }

                targets[MUL24(y, numOutputsX) + x] = prod;
//                targets[0] = prod;
            }
//            targets += !checkBounds || x < numOutputsX - 16 ? 16 : numOutputsX - x;
        }
//        targets += MUL24(3, numOutputsX);
    }
}

/*
 * This version uses block size (z, y, x) = 8x4x16.
 *
 * Each block convolves 1 image with 16 filters.
 * Use only when the filter size is > 14, otherwise use the functions that
 * cache the entire filter.
 */
template<bool checkFilterBounds, int imagesPerFilter>
__global__ void conv2_bw_nofit_4x16_2per(float* imgs, float* filters, float* targets, int imgSize, int filterSize) {
    const int shImgSizeX = 16 + 15, shImgSizeY = 4 + 3;
    __shared__ float shImg[shImgSizeY][shImgSizeX];
    __shared__ float shFilter[16][4][16];

    const int imgIdx = blockIdx.x;
    const int filtIdx = 2 * 8 * blockIdx.y + 2 * threadIdx.z;
    const int numFilters = 2 * 8 * gridDim.y;
    const int pidx = threadIdx.y * 16 + threadIdx.x; // thread's index within the 4x16 "plate" of threads
    const int tidx = threadIdx.z * 4 * 16 + pidx; // thread's index within its block
    const int numOutputsX = imgSize - filterSize + 1;
    const int numOutputs = MUL24(numOutputsX, numOutputsX);
    const int imgPixels = MUL24(imgSize, imgSize); // size of image

    const int shImgPixels = shImgSizeY * shImgSizeX; // size of shared buffer for image
    const int filterPixels = MUL24(filterSize, filterSize);
    const int loadX = tidx % (shImgSizeX);
    const int loadY = tidx / (shImgSizeX);
//    const int filterStride =  MUL24(filterPixels, stride);

    imgs += imgPixels * imgIdx + MUL24(loadY, imgSize) + loadX;
    filters += (imgIdx / imagesPerFilter) * numFilters * filterPixels + filtIdx*filterPixels + MUL24(threadIdx.y, filterSize) + threadIdx.x;
    targets += imgIdx * numFilters * numOutputs + MUL24(filtIdx, numOutputs) + MUL24(threadIdx.y, numOutputsX) + threadIdx.x;

    float* shFilterLoad = &shFilter[threadIdx.z * 2][0][pidx];
//    float* shFilterLoad2 = &shFilter[threadIdx.z * 2 + 1][0][pidx];
    float* shImgLoad = &shImg[loadY][loadX];

    for(int y = 0; y < numOutputsX; y += 4) {
        for (int x = 0; x < numOutputsX; x += 16) {
            float prod[2] = { 0, 0 };
            const bool compute = x + threadIdx.x < numOutputsX && y + threadIdx.y < numOutputsX;
            for (int fY = 0; fY < filterSize; fY += 4) {
                for (int fX = 0; fX < filterSize; fX += 16) {

                    __syncthreads();
                    shFilterLoad[0] = 0;
                    shFilterLoad[16*4] = 0;
                    if (!checkFilterBounds || (threadIdx.x + fX < filterSize && threadIdx.y + fY < filterSize)) {
                        float* f = &filters[MUL24(fY, filterSize) + fX];
                        shFilterLoad[0] = f[0];
                        shFilterLoad[16*4] = f[filterPixels];
                    }
//                    filters += !checkFilterBounds || fX < filterSize - 16 ? 16 : filterSize - fX;

                    if (tidx < shImgPixels && x + fX + loadX < imgSize && y + fY + loadY < imgSize) {
                            // I tried incrementing imgs instead of indexing it, but that
                            // uses more registers and doesn't speed things up much.
                            shImgLoad[0] = imgs[MUL24((y + fY), imgSize) + x + fX];
                    }

                    __syncthreads();

                    if (compute) {
                        float* myShFilter = &shFilter[2 * threadIdx.z][0][0];
                        float* myShImg = &shImg[threadIdx.y][threadIdx.x];
                        #pragma unroll
                        for (int i = 0; i < 4; i++) {
                            #pragma unroll
                            for (int j = 0; j < 16; j++) {
                                prod[0] += myShFilter[0] * myShImg[0];
                                prod[1] += myShFilter[16 * 4] * myShImg[0];

                                myShFilter++;
                                myShImg++;
                            }
                            myShImg += 15;
                        }
                    }
                }
            }
            if (compute) {
//                targets[0] = prod[0];
//                targets[numOutputs] = prod[1];
                targets[MUL24(y, numOutputsX) + x] = prod[0];
                targets[MUL24(y, numOutputsX) + x + numOutputs] = prod[1];
            }

//            targets += !checkOutputBounds || x < numOutputsX - 16 ? 16 : numOutputsX - x;
        }

//        targets += MUL24(3, numOutputsX);
    }
}

/*
 * This function is suitable for cases when the number of outputs is small
 * (i.e. when the filter size is nearly equal to the image size).
 * This version uses a dynamic block size. bX and bY should be set
 * to the number of outputs (bX and bY are always equal).
 * bZ should be set such that bZ*bX*bY <= 512, but it's important that each
 * block have at least (2 * bX - 1)*(2 * bY - 1) threads.
 * IMPORTANT: bZ MUST be even.
 *
 * Each block convolves 1 image with bZ*2 filters.
 *
 * This one loads the filter piecewise, even if it's very small. But this is
 * more or less ok for this routine because it nonetheless loads each filter only once.
 * This is because it always has as many threads as outputs, so it doesn't need
 * to loop to produce all the outputs.
 *
 * NOTE: 4per version is slower.
 */
template<bool checkFilterBounds, bool checkFilterIdxBounds, int imagesPerFilter, int bXY, int bZ>
__global__ void conv2_bw_nofit_dynXYZ_2per(float* imgs, float* filters, float* targets, const int imgSize, const int filterSize, const int numFilters) {
    const int shImgSizeXY = 2 * bXY - 1;
    __shared__ float shImg[shImgSizeXY][shImgSizeXY];
    __shared__ float shFilter[2 * bZ][bXY][bXY];

    const int imgIdx = blockIdx.x;
    const int filtIdx = 2 * bZ * blockIdx.y + 2 * threadIdx.z;
    const int pidx = threadIdx.y * bXY + threadIdx.x; // thread's index within the bYxbX "plate" of threads
    const int tidx = threadIdx.z * (bXY * bXY)  + pidx; // thread's index within its block
    const int imgPixels = MUL24(imgSize, imgSize); // size of image

    const int shImgPixels = shImgSizeXY * shImgSizeXY; // size of shared buffer for image
    const int filterPixels = MUL24(filterSize, filterSize);
    const int loadX = tidx % (shImgSizeXY);
    const int loadY = tidx / (shImgSizeXY);
    const bool load = tidx < shImgPixels;
    const int cmpX = imgSize - loadX, cmpY = imgSize - loadY;

    imgs += imgPixels * imgIdx + MUL24(loadY, imgSize) + loadX;
    filters += MUL24((imgIdx / imagesPerFilter), numFilters) * filterPixels + filtIdx * filterPixels + MUL24(threadIdx.y, filterSize) + threadIdx.x;
    targets += imgIdx * numFilters * (bXY * bXY) + filtIdx * (bXY * bXY) + MUL24(threadIdx.y, bXY) + threadIdx.x;

    float* shFilterLoad = &shFilter[threadIdx.z * 2][0][pidx];
    float* shImgLoad = &shImg[loadY][loadX];
//    if(imgIdx > 383)
//        return;
    const bool compute = filtIdx < numFilters;
    float prod[2] = { 0, 0 };
    for (int fY = 0; fY < filterSize; fY += bXY) {
        for (int fX = 0; fX < filterSize; fX += bXY) {

            __syncthreads();
            if (compute) {
                shFilterLoad[0] = 0;
                shFilterLoad[bXY * bXY] = 0;
                if (!checkFilterBounds || (threadIdx.x + fX < filterSize && threadIdx.y + fY < filterSize)) {
                    const float* f = &filters[MUL24(fY, filterSize) + fX];
                    shFilterLoad[0] = f[0];
                    shFilterLoad[bXY * bXY] = f[MUL24(filterSize, filterSize)]; // using filterSize here saves a register
                }
            }

            if (load && fX < cmpX && fY < cmpY) {
                shImgLoad[0] = imgs[MUL24(fY, imgSize) + fX];
            }

            __syncthreads();

            if (compute) {
                const float* myShFilter = &shFilter[2 * threadIdx.z][0][0];
                const float* myShImg = &shImg[threadIdx.y][threadIdx.x];
                #pragma unroll
                for (int i = 0; i < bXY; i++) {
                    #pragma unroll
                    for (int j = 0; j < bXY; j++) {
                        prod[0] += myShFilter[0] * myShImg[0];
                        prod[1] += myShFilter[bXY * bXY] * myShImg[0];

                        myShFilter++;
                        myShImg++;
                    }
                    myShImg += bXY - 1;
                }
            }
        }
//        imgs += MUL24(imgSize, bXY);
    }
    if (compute) {
        targets[0] = prod[0];
        targets[bXY * bXY] = prod[1];
    }
}

/* Templates need to be instantiated
   See: http://forums.nvidia.com/index.php?showtopic=31953&pid=178825&mode=threaded&start=0#entry178825
 */
__host__ void dummyTemplateInstantiator(void){

  conv2_bw_nofit_4x16_2per<true,1><<<0,0,0>>>(0,0,0,0,0);
  conv2_bw_nofit_4x16_2per<true,3><<<0,0,0>>>(0,0,0,0,0);
  conv2_bw_nofit_4x16_2per<false,1><<<0,0,0>>>(0,0,0,0,0);
  conv2_bw_nofit_4x16_2per<false,3><<<0,0,0>>>(0,0,0,0,0);

  conv2_bw_fit_4x16_1per<15,true,1><<<0,0,0>>>(0, 0, 0, 0);
  conv2_bw_fit_4x16_1per<15,true,3><<<0,0,0>>>(0, 0, 0, 0);
  conv2_bw_fit_4x16_1per<15,false,1><<<0,0,0>>>(0, 0, 0, 0);
  conv2_bw_fit_4x16_1per<15,false,3><<<0,0,0>>>(0, 0, 0, 0);

  conv2_bw_fit_4x16_1per<16,true,1><<<0,0,0>>>(0, 0, 0, 0);
  conv2_bw_fit_4x16_1per<16,true,3><<<0,0,0>>>(0, 0, 0, 0);
  conv2_bw_fit_4x16_1per<16,false,1><<<0,0,0>>>(0, 0, 0, 0);
  conv2_bw_fit_4x16_1per<16,false,3><<<0,0,0>>>(0, 0, 0, 0);

  conv2_bw_fit_4x16_1per<17,true,1><<<0,0,0>>>(0, 0, 0, 0);
  conv2_bw_fit_4x16_1per<17,true,3><<<0,0,0>>>(0, 0, 0, 0);
  conv2_bw_fit_4x16_1per<17,false,1><<<0,0,0>>>(0, 0, 0, 0);
  conv2_bw_fit_4x16_1per<17,false,3><<<0,0,0>>>(0, 0, 0, 0);

  conv2_bw_fit_4x16_1per<18,true,1><<<0,0,0>>>(0, 0, 0, 0);
  conv2_bw_fit_4x16_1per<18,true,3><<<0,0,0>>>(0, 0, 0, 0);
  conv2_bw_fit_4x16_1per<18,false,1><<<0,0,0>>>(0, 0, 0, 0);
  conv2_bw_fit_4x16_1per<18,false,3><<<0,0,0>>>(0, 0, 0, 0);

  conv2_bw_fit_4x16_1per<19,true,1><<<0,0,0>>>(0, 0, 0, 0);
  conv2_bw_fit_4x16_1per<19,true,3><<<0,0,0>>>(0, 0, 0, 0);
  conv2_bw_fit_4x16_1per<19,false,1><<<0,0,0>>>(0, 0, 0, 0);
  conv2_bw_fit_4x16_1per<19,false,3><<<0,0,0>>>(0, 0, 0, 0);

  conv2_bw_fit_4x16_1per<20,true,1><<<0,0,0>>>(0, 0, 0, 0);
  conv2_bw_fit_4x16_1per<20,true,3><<<0,0,0>>>(0, 0, 0, 0);
  conv2_bw_fit_4x16_1per<20,false,1><<<0,0,0>>>(0, 0, 0, 0);
  conv2_bw_fit_4x16_1per<20,false,3><<<0,0,0>>>(0, 0, 0, 0);

  conv2_bw_fit_4x16_2per<2,true,1><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<2,true,3><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<2,false,1><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<2,false,3><<<0,0,0>>>(0,0,0,0);

  conv2_bw_fit_4x16_2per<3,true,1><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<3,true,3><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<3,false,1><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<3,false,3><<<0,0,0>>>(0,0,0,0);

  conv2_bw_fit_4x16_2per<4,true,1><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<4,true,3><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<4,false,1><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<4,false,3><<<0,0,0>>>(0,0,0,0);

  conv2_bw_fit_4x16_2per<5,true,1><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<5,true,3><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<5,false,1><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<5,false,3><<<0,0,0>>>(0,0,0,0);

  conv2_bw_fit_4x16_2per<6,true,1><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<6,true,3><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<6,false,1><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<6,false,3><<<0,0,0>>>(0,0,0,0);

  conv2_bw_fit_4x16_2per<7,true,1><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<7,true,3><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<7,false,1><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<7,false,3><<<0,0,0>>>(0,0,0,0);

  conv2_bw_fit_4x16_2per<8,true,1><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<8,true,3><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<8,false,1><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<8,false,3><<<0,0,0>>>(0,0,0,0);

  conv2_bw_fit_4x16_2per<9,true,1><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<9,true,3><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<9,false,1><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<9,false,3><<<0,0,0>>>(0,0,0,0);

  conv2_bw_fit_4x16_2per<10,true,1><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<10,true,3><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<10,false,1><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<10,false,3><<<0,0,0>>>(0,0,0,0);

  conv2_bw_fit_4x16_2per<11,true,1><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<11,true,3><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<11,false,1><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<11,false,3><<<0,0,0>>>(0,0,0,0);

  conv2_bw_fit_4x16_2per<12,true,1><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<12,true,3><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<12,false,1><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<12,false,3><<<0,0,0>>>(0,0,0,0);

  conv2_bw_fit_4x16_2per<13,true,1><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<13,true,3><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<13,false,1><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<13,false,3><<<0,0,0>>>(0,0,0,0);

  conv2_bw_fit_4x16_2per<14,true,1><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<14,true,3><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<14,false,1><<<0,0,0>>>(0,0,0,0);
  conv2_bw_fit_4x16_2per<14,false,3><<<0,0,0>>>(0,0,0,0);

  conv2_bw_nofit_dynXYZ_2per<true,true,1,2,128><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<true,true,3,2,128><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<true,false,1,2,128><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<true,false,3,2,128><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<false,true,1,2,128><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<false,true,3,2,128><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<false,false,1,2,128><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<false,false,3,2,128><<<0,0,0>>>(0,0,0,0,0,0);

  conv2_bw_nofit_dynXYZ_2per<true,true,1,3,56><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<true,true,3,3,56><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<true,false,1,3,56><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<true,false,3,3,56><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<false,true,1,3,56><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<false,true,3,3,56><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<false,false,1,3,56><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<false,false,3,3,56><<<0,0,0>>>(0,0,0,0,0,0);

  conv2_bw_nofit_dynXYZ_2per<true,true,1,4,32><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<true,true,3,4,32><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<true,false,1,4,32><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<true,false,3,4,32><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<false,true,1,4,32><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<false,true,3,4,32><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<false,false,1,4,32><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<false,false,3,4,32><<<0,0,0>>>(0,0,0,0,0,0);

  conv2_bw_nofit_dynXYZ_2per<true,true,1,5,20><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<true,true,3,5,20><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<true,false,1,5,20><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<true,false,3,5,20><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<false,true,1,5,20><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<false,true,3,5,20><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<false,false,1,5,20><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<false,false,3,5,20><<<0,0,0>>>(0,0,0,0,0,0);

  conv2_bw_nofit_dynXYZ_2per<true,true,1,6,14><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<true,true,3,6,14><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<true,false,1,6,14><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<true,false,3,6,14><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<false,true,1,6,14><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<false,true,3,6,14><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<false,false,1,6,14><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<false,false,3,6,14><<<0,0,0>>>(0,0,0,0,0,0);

  conv2_bw_nofit_dynXYZ_2per<true,true,1,7,10><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<true,true,3,7,10><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<true,false,1,7,10><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<true,false,3,7,10><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<false,true,1,7,10><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<false,true,3,7,10><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<false,false,1,7,10><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<false,false,3,7,10><<<0,0,0>>>(0,0,0,0,0,0);

  conv2_bw_nofit_dynXYZ_2per<true,true,1,8,8><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<true,true,3,8,8><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<true,false,1,8,8><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<true,false,3,8,8><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<false,true,1,8,8><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<false,true,3,8,8><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<false,false,1,8,8><<<0,0,0>>>(0,0,0,0,0,0);
  conv2_bw_nofit_dynXYZ_2per<false,false,3,8,8><<<0,0,0>>>(0,0,0,0,0,0);

}


//}
