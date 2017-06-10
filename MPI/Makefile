# In  order  to  execute  this  "Makefile " just  type  "make "
OBJS1     = main.o mpiFunctions.o gameOfLife.o
OBJS2     = mainCreateFile.o mpiFunctions.o gameOfLife.o
SOURCE   = main.c mpiFunctions.c gameOfLife.c mainCreateFile.c
HEADER   = mpiFunctions.h gameOfLife.h
OUT1     = gameoflife
OUT2	 = createRandomFile
CC       = mpicc
FLAGS    = -g -Wall -c 
# -g  option  enables  debugging  mode
# -c flag  generates  object  code  for  separate  files
all: gameoflife createRandomFile
#  create/ compile  the  individual  files  >> separately <<

gameoflife: $(OBJS1)
	$(CC) -g $(OBJS1) -o $(OUT1) -lm

createRandomFile: $(OBJS2) 
		$(CC) -g $(OBJS2) -o $(OUT2) -lm

mainCreateFile.o: mainCreateFile.c $(HEADER)
	$(CC) $(FLAGS) mainCreateFile.c 

main.o: main.c $(HEADER)
	$(CC) $(FLAGS) main.c 

mpiFunctions.o: mpiFunctions.c mpiFunctions.h
	$(CC) $(FLAGS) mpiFunctions.c
	   
gameOfLife.o: gameOfLife.c gameOfLife.h
	$(CC) $(FLAGS) gameOfLife.c
	
#  clean  house
clean :
	rm -f $(OBJS1) $(OBJS2) $(OUT1) $(OUT2)
