.PHONY: dir clean

all: dir bin/shell

test: dir bin/test_fifo
	bin/test_fifo

bin/shell: shell.c fifo.h
	gcc $< -o $@

bin/test_fifo: test_fifo.c fifo.h
	gcc $< -o $@

dir:
	mkdir -p bin

clean:
	rm -rf bin/*
