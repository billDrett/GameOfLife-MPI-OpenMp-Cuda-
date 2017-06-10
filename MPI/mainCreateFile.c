#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "gameOfLife.h"
#include "mpiFunctions.h"

int main(int argc, char **argv)
{
	int myid, numprocs;
	int** block1;

	int TotalRows = -1, TotalColumns = -1;
	int i;
	char* filename = NULL;

    if (argc < 5 || argc > 7)
    {
        printf("Please give all attributes –r <rows> –c <columns> -f <outFileNames>\n");
        exit(1);
    }

    //get rows and columns from command line
    for(i=1; i < argc; i++)
    {
        if(i+1 != argc) //not the last element in array
        {
            if(strcmp(argv[i], "-r") == 0)
            {
                TotalRows = atoi(argv[i+1]);
            }
            else if(strcmp(argv[i], "-c") == 0)
            {
                TotalColumns = atoi(argv[i+1]);
            }
            else if(strcmp(argv[i], "-f") == 0)
            {
                filename = argv[i+1];
            }
        }
    }

    if(TotalColumns == -1 || TotalRows == -1 || filename== NULL)
    {
        printf("No Input\n");
        exit(1);
    }

    //MPI initialization
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    srand(time(NULL) ^ myid); //srand differnt value on its process with the use of myid

    //axisSize = sqrt(numprocs); //size of the cartesian topology

    //nRows = TotalRows/axisSize;
    //nColumns = TotalColumns/axisSize;

    block1 = allocate2dArray(TotalRows, TotalColumns);
    initializeBlock(block1, TotalRows, TotalColumns);

    createFile(filename, MPI_COMM_WORLD, myid, block1, TotalRows, TotalColumns);


    MPI_Finalize();

    return 0;
}
