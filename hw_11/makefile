CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -pthread
TARGET = some_shop
SRC = some_shop.c shop.c buyer.c loader.c random.c global.c
OBJ = some_shop.o shop.o buyer.o loader.o random.o global.o
HDR = shop.h buyer.h loader.h random.h global.h

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) $(LIBS)

%.o: %.c $(HDR)
	$(CC) $< -c

clean:
	rm -f $(TARGET) *.o

.PHONY: clean 
