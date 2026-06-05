CC = gcc
CFLAGS = -Wall
MAIN = main

all: $(MAIN)

$(MAIN): $(MAIN).c
	$(CC) $(CFLAGS)  $(MAIN).c -o $(MAIN)

clean:
	rm -f $(MAIN)

.PHONY: all clean 
