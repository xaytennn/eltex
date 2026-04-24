CC = gcc
CFLAGS = -Wall
LIBS_SERVER = -lrt -lpthread
LIBS_CLIENT = -lrt -lpthread -lncurses
TARGET_SERVER = server
TARGET_CLIENT = client

all: server client

server: server.c chat_defs.h
	$(CC) $(CFLAGS) server.c -o server $(LIBS_SERVER)

client: client.c chat_defs.h
	$(CC) $(CFLAGS) client.c -o client $(LIBS_CLIENT)

clean:
	rm -f server client

.PHONY: all clean 
