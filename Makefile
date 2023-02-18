CC=g++
CFLAGS=-pthread

all: server client

server: server.cpp
	$(CC) -o server server.cpp $(CFLAGS)

client: client.cpp
	$(CC) -o client client.cpp $(CFLAGS)

clean:
	rm -f server client
