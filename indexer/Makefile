CC=g++ -std=c++11
RM=rm

Objects=main.o jsmn.o lexical_analyzer.o index_manager.o

all: $(Objects)
	$(CC) -o createIndex $(Objects) -I.
	$(RM) *.o

main.o: main.cpp
	$(CC) -c main.cpp

jsmn.o: jsmn/jsmn.c
	$(CC) -c jsmn/jsmn.c

lexical_analyzer.o: lexical_analyzer.cpp
	$(CC) -c lexical_analyzer.cpp

index_manager.o: index_manager.cpp
	$(CC) -c index_manager.cpp	

clean:
	$(RM) *.o
