CC=gcc
SRC=som.c main.c -lm
OBJ=$(SRC:.c=.o)
FLAGS=-Wall

som:    $(OBJ) som.h
	$(CC) -o $@ $(OBJ)
%.o:	%.c
	$(CC) -c $< $(CFLAGS)

clean:
	rm *.o *~ core