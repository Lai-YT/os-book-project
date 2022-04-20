# Linux Kernel Modules

List all kernel modules that are currently loaded
```
lsmod
```

## Part I—Creating Kernel Modules

- Compile `simple.c` into kernel module `simple.ko`
```
make
```

### Loading and Removing Kernel Modules

1. Load kernel module
```
sudo insmod simple.ko
```
2. Remove kernel module
```
sudo rmmod simple
```
3. Check the contents of init/load message in
the kernel log buffer
```
dmesg
```
