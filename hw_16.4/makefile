CC = gcc
CFLAGS = -Wall
TARGET_CLIENT = raw_client

all: $(TARGET_CLIENT)

$(TARGET_CLIENT): $(TARGET_CLIENT).c
	$(CC) $(CFLAGS) $(TARGET_CLIENT).c -o $(TARGET_CLIENT) 

clean:
	rm -f $(TARGET_CLIENT) 

.PHONY: all clean 
