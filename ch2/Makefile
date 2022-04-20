# to kernel module
obj-m += simple.o

# Build an external module
#  -C option: cd to the specified directory and read its Makefile
#  M param: cd back after execution
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
