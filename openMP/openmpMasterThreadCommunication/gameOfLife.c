#include "gameOfLife.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

//allocates a continuius 2d array
int** allocate2dArray(int nRows, int nColumns)
{
    int i;
    int* data;
    int** anArray;


    anArray = (int**)malloc(nRows* sizeof(int*)+nRows*nColumns*sizeof(int)); //size of the pointer and size of items
    data = (int*) (anArray + nRows); //address after the array of pointers, data are continues after that address

    for(i = 0; i < nRows; i++)
    {
        anArray[i] = data + i*nColumns; //indexes point to the right memory address
    }

    return anArray;
}

//returns the number of active neighbors
int activeNeighbors(int** block, int xCoord, int yCoord, int nRows, int nColumns)
{
	int lowX, lowY, maxX, maxY;
	int activeNeigh;

	lowX = (xCoord-1+nRows)%nRows; //the up element on x axis, when -1 goes to nRows-1
	lowY = (yCoord-1 + nColumns)%nColumns; //the up element on y axis, when -1 goes to nColumns-1

	maxX = (xCoord+1)%nRows; //the down element on x axis,  when nRows goes to 0
	maxY = (yCoord+1)%nColumns; //the down element on y axis

	activeNeigh =  block[lowX][lowY]+ block[lowX][yCoord]+block[lowX][maxY] //the up 3 neighbors
    +block[xCoord][lowY]+block[xCoord] [maxY] //the middle neighbors
    +block[maxX][lowY]+ block[maxX][yCoord]+block[maxX][maxY]; //the down 3 neighbors

    return activeNeigh;
}


//returns the number of active neighbors without including the neighbors of the bounds, MPI use that function to avoid the mod calculations from the previous
int activeNeighborsNoBound(int** block, int xCoord, int yCoord)
{
    int i, j;
    int sum = 0;

    for(i=xCoord-1; i<= xCoord+1; i++)
    {
        for(j=yCoord-1; j<=yCoord+1;j++)
        {
            if(i != xCoord || j != yCoord) //its not the center element, the element which we check its neighbors
            {
                if(block[i][j]==1) sum++;
            }
        }
    }

    return sum;
}


//return the new value of the cell
int updatedValue(int oldvalue, int activNeigh)
{
    if(oldvalue ==0)//dead organization
    {
        if(activNeigh==3) return 1;
        else return 0;
    }
    else//oldvalue ==1
    {
        if(activNeigh == 2 || activNeigh==3) return 1;
        else return 0;
    }
}

//initialization of the block randomly
void initializeBlock(int** block, int nRows, int nColumns)
{
    int i,j;

    for(i = 0; i < nRows; i++)
    {
        for(j = 0; j<nColumns;j++)
        {
            block[i][j] = rand()%2; //either 0 or 1, dead or alive
        }
    }
}

//calculates the bounds of the block, except for the corner elements
void calculateBound(int** oldBlock, int** newBlock, int nRows, int nColumns, enum Bound bound)
{
    int i,j;

    //starts for 2 and size -2 because we calculate the corners later
    //the corner values cannot be calculated at the moment becauce we havent receive all the data from the neighbor process
    if(bound == UPROW)
    {
        i =1;
        for(j =2; j < nColumns-2; j++)
        {//printf("%d %d has active neighboors %d and value %d \n", i, j, activeNeighbors(oldBlock, i, j), oldBlock[i][j]);
            newBlock[i][j] = updatedValue(oldBlock[i][j], activeNeighborsNoBound(oldBlock, i, j));
        }
    }
    else if(bound == DOWNROW)
    {
        i =nRows-2;
        for(j =2; j < nColumns-2; j++)
        {
            newBlock[i][j] = updatedValue(oldBlock[i][j], activeNeighborsNoBound(oldBlock, i, j));
        }
    }
    else if(bound == LEFTCOLUMN)
    {
        j = 1;
        for(i =2; i < nRows-2; i++)
        {
            newBlock[i][j] = updatedValue(oldBlock[i][j], activeNeighborsNoBound(oldBlock, i, j));
        }

    }
    else//RIGHTCOLUMN
    {
        j = nColumns-2;
        for(i =2; i < nRows-2; i++)
        {
            newBlock[i][j] = updatedValue(oldBlock[i][j], activeNeighborsNoBound(oldBlock, i, j));
        }
    }
}

//checks if a block is changed
int blockChanged(int** prevBlock, int** currentBlock, int nRows, int nColumns)
{
    int i, j;

    for(i =1; i < nRows-1; i++)
    {
        for(j = 1; j < nColumns-1; j++)
        {
            if(currentBlock[i][j]!= prevBlock[i][j]) return 1;
        }
    }

    return 0;
}

void printBlock(int** block, int nRows, int nColumns)
{
    int i, j;

    for(i = 0; i < nRows; i++)
    {
        for(j =0; j < nColumns; j++)
        {
            printf(" %d ", block[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}
