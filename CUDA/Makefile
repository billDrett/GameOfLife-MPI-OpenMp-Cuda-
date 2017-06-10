# In  order  to  execute  this  "Makefile " just  type  "make "
OBJS     = main.o gameOfLifeGPU.o gameOfLife.o
SOURCE   = main.cpp gameOfLife.c 
HEADER   = gameOfLife.h
OUT      = gameoflife
CC       = g++
FLAGS    = -g -Wall -c 
# -g  option  enables  debugging  mode
# -c flag  generates  object  code  for  separate  files
all: $(OBJS)
	nvcc -g $(OBJS) -o $(OUT) 
#  create/ compile  the  individual  files  >> separately <<

main.o: main.cpp $(HEADER)
	$(CC) $(FLAGS) main.cpp 

gameOfLifeGPU.o: gameOfLifeGPU.cu
	nvcc -c gameOfLifeGPU.cu
	   
gameOfLife.o: gameOfLife.c gameOfLife.h
	$(CC) $(FLAGS) gameOfLife.c
	
#  clean  house
clean :
	rm -f $(OBJS) $(OUT)
