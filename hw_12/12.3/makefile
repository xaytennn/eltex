CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lreadline
TARGET = my_bash
SRC = my_bash.c parser.c bash_core.c
OBJ = my_bash.o parser.o bash_core.o 
HDR = bash_core.h parser.h

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) $(LIBS)

%.o: %.c $(HDR)
	$(CC) $< -c

clean:
	rm -f $(TARGET) *.o

.PHONY: clean 
