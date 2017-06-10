#ifndef Included_GameOfLife
#define Included_GameOfLife

enum Bound
{
    UPROW,
    DOWNROW,
    LEFTCOLUMN,
    RIGHTCOLUMN,
};

enum Positions
{
    LEFTUP,
    UP,
    RIGHTUP,
    LEFT,
    RIGHT,
    LEFTDOWN,
    DOWN,
    RIGHTDOWN,
};

int** allocate2dArray(int nRows, int nColumns);
int activeNeighbors(int** block, int xCoord, int yCoord, int nRows, int nColumns);
int activeNeighborsNoBound(int** block, int xCoord, int yCoord);
int updatedValue(int oldvalue, int activNeigh);
void initializeBlock(int** block, int nRows, int nColumns);
void calculateBound(int** oldBlock, int** newBlock, int nRows, int nColumns, enum Bound bound);
int blockChanged(int** prevBlock, int** currentBlock, int nRows, int nColumns);
void printBlock(int** block, int nRows, int nColumns);

#endif
