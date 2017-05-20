CC=g++ -std=c++11
RM=rm

indexing: main.o lexical_analyzer.o jsmn.o
	$(CC) -o indexing main.o lexical_analyzer.o jsmn.o -I.

main.o: main.cpp
	$(CC) -c main.cpp

lexical_analyzer.o: lexical_analyzer.cpp
	$(CC) -c lexical_analyzer.cpp

jsmn.o: jsmn/jsmn.c
	$(CC) -c jsmn/jsmn.c

clean:
	$(RM) *.o
