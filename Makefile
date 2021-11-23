CC = gcc
CFLAGS = -Wall -I. -g
LIBS = -lncurses -lm

EXE = donut

all:
	$(CC) $(CFLAGS) donut.c -o $(EXE) $(LIBS)