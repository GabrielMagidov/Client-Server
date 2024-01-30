CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -lpthread

all: clean StompWCIClient

EchoClient: bin/ConnectionHandler.o bin/echoClient.o
	g++ -o bin/EchoClient bin/ConnectionHandler.o bin/echoClient.o $(LDFLAGS)

StompWCIClient: bin/ConnectionHandler.o bin/game.o bin/dataBase.o bin/StompClient.o bin/event.o 
	g++ -o bin/StompWCIClient bin/ConnectionHandler.o bin/game.o bin/dataBase.o bin/StompClient.o bin/event.o $(LDFLAGS)

bin/ConnectionHandler.o: src/ConnectionHandler.cpp
	g++ $(CFLAGS) -o bin/ConnectionHandler.o src/ConnectionHandler.cpp

bin/dataBase.o: src/dataBase.cpp
	g++ $(CFLAGS) -o bin/dataBase.o src/dataBase.cpp

bin/echoClient.o: src/echoClient.cpp
	g++ $(CFLAGS) -o bin/echoClient.o src/echoClient.cpp

bin/game.o: src/game.cpp
	g++ $(CFLAGS) -o bin/game.o src/game.cpp	

bin/event.o: src/event.cpp
	g++ $(CFLAGS) -o bin/event.o src/event.cpp


bin/StompClient.o: src/StompClient.cpp
	g++ $(CFLAGS) -o bin/StompClient.o src/StompClient.cpp

bin/StompProtocol.o: src/StompProtocol.cpp
	g++ $(CFLAGS) -o bin/StompProtocol.o src/StompProtocol.cpp		

.PHONY: clean
clean:
	rm -f bin/*
	