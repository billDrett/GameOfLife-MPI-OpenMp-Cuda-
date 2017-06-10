# In  order  to  execute  this  "Makefile " just  type  "make "
OBJS     = main.o gameOfLife.o
SOURCE   = main.c gameOfLife.c 
HEADER   = gameOfLife.h
OUT      = gameoflife
CC       = gcc
FLAGS    = -g -Wall -c 
# -g  option  enables  debugging  mode
# -c flag  generates  object  code  for  separate  files
all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) 
#  create/ compile  the  individual  files  >> separately <<

main.o: main.c $(HEADER)
	$(CC) $(FLAGS) main.c
	   
gameOfLife.o: gameOfLife.c gameOfLife.h
	$(CC) $(FLAGS) gameOfLife.c
	
#  clean  house
clean :
	rm -f $(OBJS) $(OUT)
