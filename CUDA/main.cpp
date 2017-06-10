#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "gameOfLife.h"

extern "C" double gameOfLifeGPU(int** h_prevBlock, int** h_currentBlock, int nRows, int nColumns);

int main(int argc, char **argv)
{
    int** h_prevBlock;
    int** h_currentBlock;
    int i;
	int nRows = -1, nColumns = -1;
	double elapsedTime;
	
    if (argc != 5)
    {
        printf("Please give all attributes –r <rows> –c <columns>\n");
        exit(1);
    }

	srand(time(NULL));
    //get rows and columns from command line
    for(i=1; i < argc; i++)
    {
        if(i+1 != argc) //not the last element in array
        {
            if(strcmp(argv[i], "-r") == 0)
            {
                nRows = atoi(argv[i+1]);
            }
            else if(strcmp(argv[i], "-c") == 0)
            {
                nColumns = atoi(argv[i+1]);
            }
        }
    }

    if(nRows == -1 || nColumns == -1)
    {
        printf("No Input\n");
        exit(1);
    }

    h_prevBlock = allocate2dArray(nRows, nColumns);
    h_currentBlock = allocate2dArray(nRows, nColumns);
    initializeBlock(h_prevBlock, nRows, nColumns);
    
	elapsedTime = gameOfLifeGPU(h_prevBlock, h_currentBlock, nRows, nColumns); //call GPU calculations

    printf("Elapsed time %f\n", elapsedTime);
    //printBlock(h_currentBlock, nRows, nColumns); //print the final grid 
    
    free(h_prevBlock);
    free(h_currentBlock);

	return 0;
}



