CC      = gcc
CFLAGS  = -g -Wall --std=c99
RM      = rm -f

default: all

all: Ls

Ls: main.c
	$(CC) $(CFLAGS) -o ls main.c

clean veryclean:
	$(RM) ls
