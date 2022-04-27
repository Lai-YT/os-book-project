# Linux Kernel Modules

List all kernel modules that are currently loaded
```shell
lsmod
```

## Part I—Creating Kernel Modules

- Compile `simple.c` into kernel module `simple.ko`
```shell
make
```

### Loading and Removing Kernel Modules

1. Load kernel module
```shell
sudo insmod simple.ko
```
2. Remove kernel module
```shell
sudo rmmod simple
```
3. Check the contents of init/load message in
the kernel log buffer
```shell
dmesg
```

## Part II—Kernel Data Structures

### module entry point

**Create** a linked list containing five `Birthday` structure elements. \
**Traverse** the linked list and output its contents to the kernel log buffer. \
Invoke the `dmesg` command to ensure the list is properly constructed once the kernel module has been loaded.

### module exit point

**Delete** the elements from the linked list and return the *free* memory back to the kernel. \
Again, invoke the `dmesg` command to check that the list has been removed once the kernel module has been unloaded.

![terminal view of kernel module loading and log buffer](https://imgur.com/a/OMMbtNw)
