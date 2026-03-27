CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lncurses
TARGET = my_mc
SRC = my_mc.c load_panel.c draw_namelist.c enter_directory.c
OBJ = load_panel.o draw_namelist.o enter_directory.o my_mc.o
HDR = my_mc.h

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) $(LIBS)

%.o: %.c $(HDR)
	$(CC) $< -c

clean:
	rm -f $(TARGET) *.o

.PHONY: clean 
