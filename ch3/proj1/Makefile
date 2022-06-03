.PHONY: dir clean

all: dir bin/shell

bin/shell: shell.c
	gcc $< -o $@

dir:
	mkdir -p bin

clean:
	rm -rf bin/*
