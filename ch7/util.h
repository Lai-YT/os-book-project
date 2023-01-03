#ifndef UTIL_H_
#define UTIL_H_

#include <pthread.h>
#include <stdbool.h>

/* these may be any values >= 0 */
#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 3

/* instead of having an infinite loop,
  each customer makes this many requests and leave */
#define NUMBER_OF_REQUESTS 10

/* the available amount of each resource */
extern int available[NUMBER_OF_RESOURCES];

/* the maximum demand of each customer */
extern int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the amount currently allocated to each customer */
extern int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the remaining need of each customer */
extern int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

extern pthread_mutex_t resource_mutex;
extern pthread_mutex_t rand_mutex;

/**
 * @brief Reads the number of available resources from command line arguments.
 * Exits the program if the format isn't correct.
 * @note Arguments that are not in the integer form are parsed as 0.
 */
void read_available(int argc, const char *argv[]);

/**
 * @brief Enters the bank and makes `NUMBER_OF_REQUESTS` requests and releases.
 */
void *enter_bank(void *customer_num);

/** @brief Tries to request some resources and prints out the state. */
void make_request(int customer_num);

/** @brief Releases some resources and prints out the state. */
void make_release(int customer_num);

enum Status { FAILURE = -1, SUCCESS = 0 };

/**
 * @return SUCCESS (0) if successful (the request has been granted) and FAILURE
 * (-1) if unsuccessful.
 */
enum Status request_resources(int customer_num, int request[]);

/** @return SUCCESS (0), release does not fail. */
enum Status release_resources(int customer_num, int release[]);

/**
 * @brief Returns whether the system in an safe state.
 *
 * @details This is where the Banker's algorithm goes.
 */
bool is_in_safe_state(void);

/**
 * @brief Generates `NUMBER_OF_RESOURCES` random amounts of resources, each of
 * them is a non negative integer, say R_i, which is not greater than max_i.
 */
int *gen_random_resources(int max[]);

/**
 * @brief Initializes `maximum`, `need` and `allocation`. `available`s have to
 * be determined before calling this function.
 *
 * @details
 * `maximum`s are randomly set with number of instances that are not greater
 * than `available`s; `need`s are set to be as same as `maximum`s; `allocation`s
 * are set to zeros.
 */
void init_state(void);

void print_state(void);

void print_request(int request[]);
void print_release(int release[]);

#endif /* end of include guard: UTIL_H_ */
