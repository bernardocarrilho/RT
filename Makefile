CC=gcc
CFLAGS=-Wall -Wextra -g

all: main

lista_ligada.o: lista_ligada.c lista_ligada.h
	$(CC) $(CFLAGS) -c lista_ligada.c

main: main.c lista_ligada.o
	$(CC) $(CFLAGS) -o $@ $^ -lm
     
clean:
	rm -f *.o main
