CC=g++ -std=c++11
RM=rm

indexing: indexing.o jsmn.o
	$(CC) -o indexing indexing.o jsmn.o -I.

indexing.o: indexing.cpp jsmn/jsmn.h THULAC/include/*
	$(CC) -c indexing.cpp

jsmn.o: jsmn/jsmn.c jsmn/jsmn.h
	$(CC) -c jsmn/jsmn.c

clean:
	$(RM) *.o
