CC = gcc
CFLAGS = -Wall
LIBS_SERVER = -lpthread
LIBS_CLIENT = -lpthread
TARGET_SERVER = server
TARGET_CLIENT = client

all: server client

$(TARGET_SERVER): $(TARGET_SERVER).c
	$(CC) $(CFLAGS) server.c -o server $(LIBS_SERVER)

$(TARGET_CLIENT): $(TARGET_CLIENT).c
	$(CC) $(CFLAGS) client.c -o client $(LIBS_CLIENT)

clean:
	rm -f server client

.PHONY: all clean 
