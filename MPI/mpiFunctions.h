#ifndef Included_MpiFunctions
#define Included_MpiFunctions

#include "mpi.h"
#include "gameOfLife.h"

void neighborProcess(MPI_Comm communicator,int processRank, int* neighbors);
void sendMSG(int neighborRank, enum Positions blockPosRequest, int** block, int nRows, int nColumns, MPI_Request* sendRequestId, MPI_Comm communicator, MPI_Datatype ROW, MPI_Datatype COLUMN);
void receiveMSG(int neighborRank, enum Positions blockPosRequest, int** block, int nRows, int nColumns, MPI_Request* receiveRequestId, MPI_Comm communicator, MPI_Datatype ROW, MPI_Datatype COLUMN);
void createCartesianTopology(MPI_Comm* gridComm, int axisSize);
void readFromFile(char* fileName, MPI_Comm communicator, int processRank, int** block, int nRows, int nColumns, int totalRows, int totalColumns);
void createFile(char* fileName, MPI_Comm communicator, int processRank, int** block, int nRows, int nColumns);

#endif
