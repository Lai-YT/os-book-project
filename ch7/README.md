# Banker's Algorithm

For this project, you will write a multithreaded program that implements the banker's algorithm (`is_in_safe_state`). Several customers request (`request_resources`) and release resources (`release_resources`) from the bank. \
The banker will grant a request only if it leaves the system in a safe state. A request that leaves the system in an unsafe state will be denied. \
This programming assignment combines three separate topics:

- (1) multithreading
- (2) preventing race conditions
- (3) deadlock avoidance

## How to use?

```bash
$ make

# available number of each resources
$ bin/bank 10 5 7
```
