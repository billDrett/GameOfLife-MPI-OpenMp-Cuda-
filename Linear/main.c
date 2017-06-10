#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "../timers/timer.h"
#include "gameOfLife.h"
#define NOLOOPS 100

int main(int argc, char **argv)
{
    int** prevBlock;
    int** currentBlock;
    int** tempBlock;
    int i,j,k;

	int nRows, nColumns;
	double startTime, finishTime, elapsedTime;

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

    prevBlock = allocate2dArray(nRows, nColumns);
    currentBlock = allocate2dArray(nRows, nColumns);

    initializeBlock(prevBlock, nRows, nColumns);

    GET_TIME(startTime);
    for(k =0; k <NOLOOPS; k++)
    {
        for(i =0; i < nRows; i++)
        {
            for(j =0; j <nColumns; j++)
            {
                currentBlock[i][j] = updatedValue(prevBlock[i][j], activeNeighbors(prevBlock, i, j, nRows, nColumns));
            }
        }

		//printBlock(prevBlock, nRows, nColumns);

        tempBlock = prevBlock;
        prevBlock = currentBlock;
        currentBlock = tempBlock;
    }

    GET_TIME(finishTime);
    elapsedTime = finishTime-startTime;

    printf("Elapsed time %f\n", elapsedTime);
    free(prevBlock);
    free(currentBlock);

    return 0;
}
