CC = gcc
CFLAGS = -Wall -Wextra
TARGET = calc
LIBRARY = libcalc.a
AR = ar
ARFLAGS = rc
LIB_OBJS = add.o sub.o mul.o div.o

$(TARGET): main.o $(LIBRARY)
	$(CC) main.o -L. -lcalc -o $(TARGET)

$(LIBRARY): $(LIB_OBJS)
	$(AR) $(ARFLAGS) $(LIBRARY) $(LIB_OBJS)

add.o: add.c calc.h
	$(CC) $(CFLAGS) -c add.c

sub.o: sub.c calc.h
	$(CC) $(CFLAGS) -c sub.c

mul.o: mul.c calc.h
	$(CC) $(CFLAGS) -c mul.c

div.o: div.c calc.h
	$(CC) $(CFLAGS) -c div.c

main.o: main.c calc.h
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f $(LIB_OBJS) main.o $(LIBRARY) $(TARGET)
	
.PHONY: clean
