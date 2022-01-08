
CC=gcc -Wall
  
all: main
main.o: main.c
	$(CC) -c -DLARGEUR=20 -DDELAY=20000 main.c
main: main.o
	$(CC) main.o -o main -lncurses

clean:
	rm main main.o 
