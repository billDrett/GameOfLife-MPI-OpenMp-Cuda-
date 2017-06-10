#ifndef _CUTIL_H_
#define _CUTIL_H_

#include <stdio.h>
#include <cuda.h>

#  define CUDA_SAFE_CALL( call) {                                    \
    cudaError err = call;                                                    \
    if( cudaSuccess != err) {                                                \
        fprintf(stderr, "Cuda error in file '%s' in line %i : %s.\n",        \
                __FILE__, __LINE__, cudaGetErrorString( err) );              \
        exit(EXIT_FAILURE);                                                  \
    } }

#define FRACTION_CEILING(numerator, denominator) ((numerator+denominator-1)/denominator)

#endif

