#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "gameOfLife.h"
#include "mpiFunctions.h"
#define NLOOPS 100


int main(int argc, char **argv)
{
    MPI_Datatype ROW, COLUMN;
	int myid, numprocs;
	MPI_Status recv_status, send_status;
	MPI_Request sendReq[2][8];
	MPI_Request receiveReq[2][8];

	MPI_Comm gridComm;
	int** block1;
	int** block2;
	int** currentBlock;
	int** prevBlock;
	int** tempBlock;


	int axisSize;
	int neighbors[8];
	int nRows, nColumns, TotalRows = -1, TotalColumns = -1;
	int i, j, k;
	int change, GlobalChange;
	int finReqstId;
	char* filename = NULL;
	double startTime, finishTime, localElapsed, globalElapsed;
	int pos;

    if (argc < 5 || argc > 7)
    {
        printf("Please give all attributes –r <rows> –c <columns> -f <filename(optimal)>\n");
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

    if(TotalColumns == -1 || TotalRows == -1)
    {
        printf("No Input\n");
        exit(1);
    }

    //MPI initialization
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    srand(time(NULL) ^ myid); //srand differnt value on its process with the use of myid

    axisSize = sqrt(numprocs); //size of the cartesian topology

    nRows = TotalRows/axisSize;
    nColumns = TotalColumns/axisSize;


	if(TotalRows%axisSize != 0 || TotalColumns%axisSize !=0)
    {
        printf("rows or columns cannot be divided equality to all process\n");
        MPI_Finalize();
        exit(1);
    }

    createCartesianTopology(&gridComm, axisSize); //create cartesian topology

    //create datatype
    MPI_Type_contiguous(nColumns, MPI_INT, &ROW); //row datatype
    MPI_Type_commit(&ROW);
    MPI_Type_vector(nRows, 1, nColumns +2, MPI_INT, &COLUMN); //column datatype, +2 because we have 2 more columns for the received data
    MPI_Type_commit(&COLUMN);

    //The first and last row and column are used for the received data
    //so the data of the table are in bounds [1,size-2], so we increase the rows and columns by 2

    nRows += 2;
    nColumns +=2;

    //dynamic allocation of two arrays
    block1 = allocate2dArray(nRows, nColumns);
    block2 = allocate2dArray(nRows, nColumns);

    MPI_Comm_rank(gridComm, &myid); //rank in new communicator

    if(filename == NULL) //random initialization
    {
        printf("Random initialization\n");
        initializeBlock(block1, nRows, nColumns);
    }
    else //initialization from file
    {
        printf("Input from file\n");
        readFromFile(filename, gridComm, myid, block1, nRows-2, nColumns -2, TotalRows, TotalColumns);
    }

    neighborProcess(gridComm, myid, neighbors); //gets the neighbor process

    prevBlock = block1;
    currentBlock = block2;

    //initialization of receive and send messages
    for(j=0; j < 8; j++)
    {
        //send and receive for block1 memory
        sendMSG(neighbors[j], j, prevBlock, nRows, nColumns, &sendReq[1][j], gridComm, ROW, COLUMN);
        receiveMSG(neighbors[j], j, prevBlock, nRows, nColumns, &receiveReq[1][j], gridComm, ROW, COLUMN);

        //send and receive for block2 memory
        sendMSG(neighbors[j], j, currentBlock, nRows, nColumns, &sendReq[0][j], gridComm, ROW, COLUMN);
        receiveMSG(neighbors[j], j, currentBlock, nRows, nColumns, &receiveReq[0][j], gridComm, ROW, COLUMN);
    }

    MPI_Barrier(gridComm); //wait all process to come here
    startTime = MPI_Wtime();

    for(k =1; k<= NLOOPS; k++)
    {
        pos = k%2; //find which send request to use of block1 or block2, we start with prevBlock block1, so pos must be 1 the first time

        for(i=0; i < 8; i++) //send the requests to neighbors
        {
            MPI_Start(&sendReq[pos][i]);
        }

        for(i=0; i < 8; i++) //send requests for receive data to neigbors
        {
            MPI_Start(&receiveReq[pos][i]);
        }

        for(i = 2; i < nRows-2; i++) //calculate data inside the 2d array where we have all the data to find the new value of the cell
        {
            for(j = 2; j <nColumns-2; j++)
            {
                currentBlock[i][j] = updatedValue(prevBlock[i][j], activeNeighborsNoBound(prevBlock, i, j));
            }
        }

        for(i=0; i < 8; i++)
        {
            MPI_Waitany(8, receiveReq[pos], &finReqstId, &recv_status);
            if(finReqstId == UP)
            {
                calculateBound(prevBlock, currentBlock, nRows, nColumns, UPROW);
            }
            else if(finReqstId == DOWN)
            {
                calculateBound(prevBlock, currentBlock, nRows, nColumns, DOWNROW);
            }
            else if(finReqstId == LEFT)
            {
                calculateBound(prevBlock, currentBlock, nRows, nColumns, LEFTCOLUMN);
            }
            else if(finReqstId == RIGHT)
            {
                calculateBound(prevBlock, currentBlock, nRows, nColumns, RIGHTCOLUMN);
            }
        }

        //calculate corners
        currentBlock[1][1] = updatedValue(prevBlock[1][1], activeNeighborsNoBound(prevBlock, 1, 1)); //top left
        currentBlock[1][nColumns-2] = updatedValue(prevBlock[1][nColumns-2], activeNeighborsNoBound(prevBlock, 1, nColumns-2)); //top right
        currentBlock[nRows-2][1] = updatedValue(prevBlock[nRows-2][1], activeNeighborsNoBound(prevBlock, nRows-2, 1)); //bottom left
        currentBlock[nRows-2][nColumns-2] = updatedValue(prevBlock[nRows-2][nColumns-2], activeNeighborsNoBound(prevBlock, nRows-2, nColumns-2)); //botum right

        for(i=0; i <8; i++) //wait for the finish of all the send requests
        {
            MPI_Wait(&sendReq[pos][i], &send_status);
        }
/*
        //every 5 loops check if all cells are dead
        if(k%5 == 0)
        {
            change = blockChanged(prevBlock, currentBlock, nRows, nColumns);
            MPI_Allreduce(&change, &GlobalChange, 1, MPI_INT, MPI_LOR, gridComm);
            if(GlobalChange == 0)
            {
                printf("No change after steps %d\n", k);
                break;
            }
        }
*/

        tempBlock = prevBlock;
        prevBlock = currentBlock; //change the prev array to the current
        currentBlock = tempBlock; //at the new values will be save to the other array

        //printBlock(prevBlock, nRows, nColumns);
    }

    finishTime = MPI_Wtime();
    localElapsed = finishTime - startTime;

    MPI_Reduce(&localElapsed, &globalElapsed, 1, MPI_DOUBLE, MPI_MAX, 0, gridComm);
    if(myid == 0)
    {
        printf("Total time is %f\n", globalElapsed);
    }

    //free resources
    MPI_Comm_free(&gridComm);
    MPI_Type_free(&ROW);
    MPI_Type_free(&COLUMN);
	MPI_Finalize();

	free(block1);
	free(block2);

	exit(0);
}
