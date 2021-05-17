  
CC = gcc
FLAGS = -Wall -g
OBJS = main.c F1simulator.c RaceManager.c MalfunctionManager.c TeamManager.c log.c -lpthread
TARGET = f1

all: ${TARGET}


${TARGET} : ${OBJS}
			${CC} ${FLAGS} ${OBJS} -o ${TARGET}


clean:
		rm *.o

# rm -f *.o log.txt ${TARGET} *~