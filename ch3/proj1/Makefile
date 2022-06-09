.PHONY: dir clean

all: dir bin/shell

bin/shell: shell.c fifo.h
	gcc $< -o $@

bin/test_fifo: test_fifo.c fifo.h
	gcc $< -o $@

dir:
	mkdir -p bin

clean:
	rm -rf bin/*
