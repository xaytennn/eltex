CC = gcc
CFLAGS = -Wall
TARGET_SERVER = server
TARGET_UDP_CLIENT = udp_client
TARGET_TCP_CLIENT = tcp_client

all: server udp_client tcp_client

$(TARGET_SERVER): $(TARGET_SERVER).c
	$(CC) $(CFLAGS) server.c -o server 

$(TARGET_UDP_CLIENT): $(TARGET_UDP_CLIENT).c
	$(CC) $(CFLAGS) udp_client.c -o udp_client 

$(TARGET_TCP_CLIENT): $(TARGET_TCP_CLIENT).c
	$(CC) $(CFLAGS) tcp_client.c -o tcp_client 

clean:
	rm -f $(TARGET_SERVER) $(TARGET_UDP_CLIENT) $(TARGET_TCP_CLIENT)

.PHONY: all clean 
