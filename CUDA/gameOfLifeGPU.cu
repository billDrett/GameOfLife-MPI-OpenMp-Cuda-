#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <cuda.h>
#include "../timers/timer.h"

//#include <cuda_runtime_api.h>
#define NOLOOPS 100

__global__ void updatedValueGPU(int* prevBlock, int* currentBlock, int nRows, int nColumns)
{
	const unsigned int i = blockIdx.x * blockDim.x + threadIdx.x;
	int xCoord;
	int yCoord;
	int lowX, lowY, maxX, maxY;
	int activeNeigh;

	if(i< nRows*nColumns)
	{
        xCoord = i/nColumns;
        yCoord = i-xCoord*nColumns;

        lowX = nColumns*((xCoord-1+nRows)%nRows); //the up element on x axis, when -1 goes to nRows-1
        lowY = (yCoord-1 + nColumns)%nColumns; //the up element on y axis, when -1 goes to nColumns-1

        maxX = nColumns*((xCoord+1)%nRows); //the down element on x axis,  when nRows goes to 0
        maxY = (yCoord+1)%nColumns; //the down element on y axis

        activeNeigh =  prevBlock[lowX+lowY]+ prevBlock[lowX+yCoord]+prevBlock[lowX+maxY] //the up 3 neighbors
        +prevBlock[xCoord*nColumns+lowY]+prevBlock[xCoord*nColumns+ maxY] //the middle neighbors
        +prevBlock[maxX+lowY]+ prevBlock[maxX+yCoord]+prevBlock[maxX+maxY]; //the down 3 neighbors


		if(activeNeigh==3 || (activeNeigh==2 && prevBlock[xCoord*nColumns+yCoord] == 1)) //find new value of the cell
		{
			currentBlock[xCoord*nColumns+yCoord]= 1;
		}
		else
		{
			currentBlock[xCoord*nColumns+yCoord] = 0;
		}
	}

}

extern "C" double gameOfLifeGPU(int** h_prevBlock, int** h_currentBlock, int nRows, int nColumns)
{
    int* d_prevBlock;
    int* d_currentBlock;
    int* tempBlock;

	double startTime, finishTime, elapsedTime;
	int memSize;
	const int BLOCK_SIZE = 256;
	int N = nRows*nColumns;
	int k;

    memSize = nRows*nColumns*sizeof(int);

    cudaMalloc((void**)&d_prevBlock, memSize); //allocate GPU memory
	cudaMalloc((void**)&d_currentBlock, memSize);

	cudaMemcpy(d_prevBlock, &h_prevBlock[0][0], memSize, cudaMemcpyHostToDevice); //copy CPU memory to GPU

	GET_TIME(startTime);//start time
	
    for(k =0; k <NOLOOPS; k++) //calculate the grid for NOLOOPS
    {
		updatedValueGPU<<<(N+BLOCK_SIZE-1)/BLOCK_SIZE, BLOCK_SIZE>>>(d_prevBlock, d_currentBlock, nRows, nColumns);

		if(cudaGetLastError() !=cudaSuccess)
		{
			printf("Error in kernel code\n");
			exit(EXIT_FAILURE);
		}

        tempBlock = d_prevBlock;
        d_prevBlock = d_currentBlock;
        d_currentBlock = tempBlock;
    }
	cudaThreadSynchronize();
	
	GET_TIME(finishTime);
	elapsedTime = finishTime-startTime;

	cudaMemcpy(&h_currentBlock[0][0], d_prevBlock, memSize, cudaMemcpyDeviceToHost); //copy the results to the CPU memory

    cudaFree(d_prevBlock); //free memory
	cudaFree(d_currentBlock);

	return elapsedTime;

}
