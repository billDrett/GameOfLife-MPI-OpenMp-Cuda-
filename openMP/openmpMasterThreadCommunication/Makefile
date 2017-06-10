# In  order  to  execute  this  "Makefile " just  type  "make "
OBJS     = main.o mpiFunctions.o gameOfLife.o
SOURCE   = main.c mpiFunctions.c gameOfLife.c
HEADER   = mpiFunctions.h gameOfLife.h
OUT      = gameoflife
CC       = mpicc
FLAGS    = -g -Wall -c -fopenmp
# -g  option  enables  debugging  mode
# -c flag  generates  object  code  for  separate  files
all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) -lm 	-fopenmp
#  create/ compile  the  individual  files  >> separately <<

main.o: main.c $(HEADER)
	$(CC) $(FLAGS) main.c 

mpiFunctions.o: mpiFunctions.c mpiFunctions.h
	$(CC) $(FLAGS) mpiFunctions.c
	   
gameOfLife.o: gameOfLife.c gameOfLife.h
	$(CC) $(FLAGS) gameOfLife.c
	
#  clean  house
clean :
	rm -f $(OBJS) $(OUT)
