.PHONY: dir clean

CC = gcc
CFLAGS = -pthread
OBJ = obj/util.o

all: dir bin/bank

bin/bank: bank.c $(OBJ)
	$(CC) -o $@ $< $(OBJ) $(CFLAGS)

obj/%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

dir:
	mkdir -p bin obj

clean:
	rm -rf bin/* obj/*
