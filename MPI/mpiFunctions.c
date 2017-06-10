#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "mpiFunctions.h"
#include "gameOfLife.h"

void createFile(char* fileName, MPI_Comm communicator, int processRank, int** block, int nRows, int nColumns)
{
    int i,j;
    MPI_File fileDesc;

    MPI_File_open(communicator, fileName, MPI_MODE_CREATE|MPI_MODE_WRONLY, MPI_INFO_NULL, &fileDesc);

    if(processRank == 0)
    {
        for(i = 0; i < nRows; i++)
        {
            for(j =0; j < nColumns; j++)
            {
                MPI_File_write(fileDesc, &block[i][j], 1, MPI_INT, MPI_STATUS_IGNORE);
            }
        }

    }
    MPI_File_close(&fileDesc);


    if(processRank == 0)
    {
        printBlock(block, nRows, nColumns);
    }
}

void readFromFile(char* fileName, MPI_Comm communicator, int processRank, int** block, int nRows, int nColumns, int totalRows, int totalColumns)
{
    MPI_File fileDesc;
    MPI_Datatype READTYPE;
    int totalDimensionSize[2];
    int localDimensionSize[2];
    int startIndex[2];
    int processCoord[2];
    int i;

    MPI_File_open(communicator, fileName, MPI_MODE_RDONLY, MPI_INFO_NULL, &fileDesc);

    totalDimensionSize[0] = totalRows;
    totalDimensionSize[1] = totalColumns;

    localDimensionSize[0] = nRows;
    localDimensionSize[1] = nColumns;

    MPI_Cart_coords(communicator, processRank, 2, processCoord); //get the coordincance of the process

    startIndex[0] = processCoord[0] * localDimensionSize[0];
    startIndex[1] = processCoord[1] * localDimensionSize[1];


    MPI_Type_create_subarray(2, totalDimensionSize, localDimensionSize, startIndex, MPI_ORDER_C, MPI_INT, &READTYPE);
    MPI_Type_commit(&READTYPE);

    MPI_File_set_view(fileDesc, 0, MPI_INT, READTYPE, "native", MPI_INFO_NULL);

    for(i =1; i < nRows+1; i++)
    {
        MPI_File_read_all(fileDesc, &block[i][1], nColumns, MPI_INT, MPI_STATUS_IGNORE);
    }

    //MPI_File_read_all(fileDesc, &block[0][0], nRows*nColumns, MPI_INT, MPI_STATUS_IGNORE);
    MPI_File_close(&fileDesc);
    MPI_Type_free(&READTYPE);
}

//creates cartesian topology
void createCartesianTopology(MPI_Comm* gridComm, int axisSize)
{
    int nDims, reOrder;
	int dimSize[2];
	int periods[2];

    nDims = 2; //number of dimensions
	dimSize[0] = axisSize; //size of x axis
	dimSize[1] = axisSize; //size of y axis
	periods[0] = 1; //periodical true
	periods[1] = 1; //periodical true
	reOrder = 1; //reodred true

    MPI_Cart_create(MPI_COMM_WORLD, nDims, dimSize, periods, reOrder, gridComm);
}
//returns the neighbors of the process in all 8 directions
void neighborProcess(MPI_Comm communicator,int processRank, int* neighbors)
{
    int processCoord[2];
    int tempCoord[2];
    int i,j, pos;
    int rank;

    MPI_Cart_coords(communicator, processRank, 2, processCoord); //get the coordincance of the process

    pos = 0;
    for(i =processCoord[0]-1; i <=processCoord[0]+1; i++) // check the neighbors
    {
        for(j = processCoord[1]-1; j <=processCoord[1]+1; j++)
        {
            if(i != processCoord[0] || j != processCoord[1]) //its not the center element, the element which we check its neighbors
            {
                tempCoord[0] = i;
                tempCoord[1] = j;

                MPI_Cart_rank(communicator, tempCoord, &rank); //find the rank of the neighbor process
                neighbors[pos] = rank;

                pos++;
            }
        }
    }
}

//sends the data to the neighbor processes
void sendMSG(int neighborRank, enum Positions blockPosRequest, int** block, int nRows, int nColumns, MPI_Request* sendRequestId, MPI_Comm communicator, MPI_Datatype ROW, MPI_Datatype COLUMN)
{
    //The first and last row and column are used for the received data
    //so the data of the table are in bounds [1,size-2], thats why instead of [0][0] we send [1][1]
    if(blockPosRequest == LEFTUP) //send up left corner
    {
        MPI_Send_init(&block[1][1], 1, MPI_INT, neighborRank, RIGHTDOWN, communicator, sendRequestId);
    }
    else if(blockPosRequest == UP) //send up row
    {
        MPI_Send_init(&block[1][1], 1, ROW, neighborRank, DOWN, communicator, sendRequestId);
    }
    else if(blockPosRequest == RIGHTUP) //right up left corner
    {
        MPI_Send_init(&block[1][nColumns-2], 1, MPI_INT, neighborRank, LEFTDOWN, communicator, sendRequestId);
    }
    else if(blockPosRequest == LEFT) //send up left column
    {
        MPI_Send_init(&block[1][1], 1, COLUMN, neighborRank, RIGHT, communicator, sendRequestId);
    }
    else if(blockPosRequest == RIGHT) //send up right column
    {
        MPI_Send_init(&block[1][nColumns-2], 1, COLUMN, neighborRank, LEFT, communicator, sendRequestId);
    }
    else if(blockPosRequest == LEFTDOWN) //send down left corner
    {
        MPI_Send_init(&block[nRows-2][1], 1, MPI_INT, neighborRank, RIGHTUP, communicator, sendRequestId);
    }
    else if(blockPosRequest == DOWN) //send down row
    {
        MPI_Send_init(&block[nRows-2][1], 1, ROW, neighborRank, UP, communicator, sendRequestId);
    }
    else if(blockPosRequest == RIGHTDOWN) //send down right corner
    {
        MPI_Send_init(&block[nRows-2][nColumns-2], 1, MPI_INT, neighborRank, LEFTUP, communicator, sendRequestId);
    }
}

void receiveMSG(int neighborRank, enum Positions blockPosRequest, int** block, int nRows, int nColumns, MPI_Request* receiveRequestId, MPI_Comm communicator, MPI_Datatype ROW, MPI_Datatype COLUMN)
{
    //The first and last row and column are used for the received data
    if(blockPosRequest == LEFTUP) //receive left up corner
    {
        MPI_Recv_init(&block[0][0], 1, MPI_INT, neighborRank, LEFTUP, communicator, receiveRequestId);
    }
    else if(blockPosRequest == UP) //receive up row
    {
        MPI_Recv_init(&block[0][1], 1, ROW, neighborRank, UP, communicator, receiveRequestId);
        //
    }
    else if(blockPosRequest == RIGHTUP) //receive right up corner
    {
        MPI_Recv_init(&block[0][nColumns-1], 1, MPI_INT, neighborRank, RIGHTUP, communicator, receiveRequestId);
    }
    else if(blockPosRequest == LEFT) //receive left column
    {
        MPI_Recv_init(&block[1][0], 1, COLUMN, neighborRank, LEFT, communicator, receiveRequestId);
    }
    else if(blockPosRequest == RIGHT) //receive right column
    {
        MPI_Recv_init(&block[1][nColumns-1], 1, COLUMN, neighborRank, RIGHT, communicator, receiveRequestId);
    }
    else if(blockPosRequest == LEFTDOWN) //receive left down corner
    {
        MPI_Recv_init(&block[nRows-1][0], 1, MPI_INT, neighborRank, LEFTDOWN, communicator, receiveRequestId);
    }
    else if(blockPosRequest == DOWN) //receive down row
    {
        MPI_Recv_init(&block[nRows-1][1], 1, ROW, neighborRank, DOWN, communicator, receiveRequestId);
    }
    else if(blockPosRequest == RIGHTDOWN) //receive right down corner
    {
        MPI_Recv_init(&block[nRows-1][nColumns-1], 1, MPI_INT, neighborRank, RIGHTDOWN, communicator, receiveRequestId);
    }
}
