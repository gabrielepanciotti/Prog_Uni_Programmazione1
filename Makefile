CC=gcc
CFLAGS=-Wall -std=c11
LDLIBS=-lc
game:
	$(CC) -c main.c $(CFLAGS) $(LDLIBS)
	$(CC) -c gamelib.c $(CFLAGS) $(LDLIBS)
	$(CC) main.o gamelib.o -o gioco $(CFLAGS) $(LDLIBS)
	rm *.o
clean:
	rm gioco