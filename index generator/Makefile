CC=gcc
RM=rm

indexing: indexing.o jsmn.o
	$(CC) -o indexing indexing.o jsmn.o -I.

indexing.o: indexing.c jsmn/jsmn.h
	$(CC) -c indexing.c

jsmn.o: jsmn/jsmn.c jsmn/jsmn.h
	$(CC) -c jsmn/jsmn.c

clean:
	$(RM) *.o
