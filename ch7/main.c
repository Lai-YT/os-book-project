#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* these may be any values >= 0 */
#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 3

/* instead of having an infinite loop,
  each customer makes this many requests and leave */
#define NUMBER_OF_REQUESTS 10

/* the available amount of each resource */
int available[NUMBER_OF_RESOURCES] = {10, 5, 7}; /* read from input later */

/* the maximum demand of each customer */
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the amount currently allocated to each customer */
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the remaining need of each customer */
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

pthread_mutex_t resource_mutex;
pthread_mutex_t rand_mutex;

/**
 * @brief Initializes `maximum`, `need` and `allocation`. `available`s have to
 * be determined before calling this function.
 *
 * @details
 * `maximum`s are randomly set with number of instances that are not greater
 * than `available`s; `need`s are set to be as same as `maximum`s; `allocation`s
 * are set to zeros.
 */
void init_states(void);

void print_states(void);

enum Status { FAILURE = -1, SUCCESS = 0 };

/**
 * @return 0 if successful (the request has been granted) and –1 if
 * unsuccessful.
 */
int request_resources(int customer_num, int request[]);

/**
 * @return 0
 */
int release_resources(int customer_num, int release[]);

void print_request(int request[]);
void print_release(int release[]);

/** @brief This is where the Banker's algorithm goes. */
bool is_safe(void);

/**
 * @brief Generates `NUMBER_OF_RESOURCES` random amounts of resources, each of
 * them is a non negative integer, say R_i, which is not greater than max_i.
 */
int *gen_random_resources(int max[]);

void *enter_bank(void *customer_num);

int main(int argc, const char *argv[]) {
  srand(time(NULL));

  pthread_mutex_init(&resource_mutex, NULL);
  pthread_mutex_init(&rand_mutex, NULL);

  init_states();

  pthread_t customers[NUMBER_OF_CUSTOMERS];
  /* NOTE: customer_nums are passed as pointers, so we have to keep them
    untouched in another place. I've tried to passed the one incrementing with the
    loop, which makes all the threads use the same customer_num and breaks. */
  int customer_nums[NUMBER_OF_CUSTOMERS];
  for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
    customer_nums[i] = i;
  }
  for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
    pthread_create(&customers[i], NULL, enter_bank, &customer_nums[i]);
  }

  for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
    pthread_join(customers[i], NULL);
  }

  print_states();

  pthread_mutex_destroy(&resource_mutex);
  pthread_mutex_destroy(&rand_mutex);
  return 0;
}

void *enter_bank(void *customer_num_) {
  const int customer_num = *((int *)customer_num_);
  for (int i = 0; i < NUMBER_OF_REQUESTS; i++) {
    pthread_mutex_lock(&resource_mutex);
    int *request = gen_random_resources(need[customer_num]);
    if (request_resources(customer_num, request) == -1) {
      /* TODO: the customer has to wait until the request is granted. */
      printf("[REQUEST DENIED ON CUSTOMER %d]\n", customer_num);
    } else {
      printf("[REQUEST GRANTED ON CUSTOMER %d]\n", customer_num);
    }
    print_request(request);
    free(request);

    print_states();
    printf("####\n");
    pthread_mutex_unlock(&resource_mutex);

    pthread_mutex_lock(&resource_mutex);
    int *release = gen_random_resources(allocation[customer_num]);
    release_resources(customer_num, release);
    printf("[RELEASE BY CUSTOMER %d]\n", customer_num);
    print_release(release);
    free(release);

    print_states();
    printf("####\n");
    pthread_mutex_unlock(&resource_mutex);
  }
  pthread_exit(NULL);
}

void grant_request(int customer_num, int request[]);

int request_resources(int customer_num, int request[]) {
  grant_request(customer_num, request);
  if (!is_safe()) {
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
      allocation[customer_num][i] -= request[i];
      available[i] += request[i];
      need[customer_num][i] += request[i];
    }
    return FAILURE;
  }
  return SUCCESS;
}

void grant_request(int customer_num, int request[]) {
  for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
    allocation[customer_num][i] += request[i];
    available[i] -= request[i];
    need[customer_num][i] -= request[i];
  }
}

int release_resources(int customer_num, int release[]) {
  for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
    allocation[customer_num][i] -= release[i];
    available[i] += release[i];
    need[customer_num][i] += release[i];
  }
  return SUCCESS; /* release does not fail */
}

int *gen_random_resources(int max[]) {
  int *amount = malloc(sizeof(int) * NUMBER_OF_RESOURCES);
  pthread_mutex_lock(&rand_mutex);
  for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
    amount[i] = max[i] ? rand() % max[i] : 0;
  }
  pthread_mutex_unlock(&rand_mutex);
  return amount;
}

void init_states(void) {
  for (int customer_num = 0; customer_num < NUMBER_OF_CUSTOMERS;
       customer_num++) {
    int *max = gen_random_resources(available);
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
      maximum[customer_num][i] = max[i];
      allocation[customer_num][i] = 0;
      need[customer_num][i] = max[i]; /* no resource allocated yet */
    }
    free(max);
  }
}

void print_states(void) {
  printf("The current states is:\n"
         "     Allocation      Max       Available\n"
         "    ------------  ----------  ------------\n"
         "      A   B   C    A   B   C    A   B   C \n");
  for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
    printf("%2d: %3d %3d %3d  %3d %3d %3d", i, allocation[i][0],
           allocation[i][1], allocation[i][2], maximum[i][0], maximum[i][1],
           maximum[i][2]);
    if (i == 0) { /* available is only print once in the first row */
      printf("  %3d %3d %3d", available[0], available[1], available[2]);
    }
    printf("\n");
  }
}

void print_request(int request[]) {
  printf("The request is:\n");
  printf("  A   B   C\n");
  printf("%3d %3d %3d\n", request[0], request[1], request[2]);
}

void print_release(int release[]) {
  printf("The release is:\n");
  printf("  A   B   C\n");
  printf("%3d %3d %3d\n", release[0], release[1], release[2]);
}

bool has_resources_to_work(int work[], int need[]);

bool is_safe(void) {
  /* step 1. initialize the safety check */
  int work[NUMBER_OF_RESOURCES];
  for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
    work[i] = available[i];
  }
  bool finish[NUMBER_OF_CUSTOMERS];
  for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
    finish[i] = false;
  }

  /* step 2. find a customer to work on */
  int customer_num = 0;
  while (customer_num != NUMBER_OF_CUSTOMERS) {
    if (!finish[customer_num] &&
        has_resources_to_work(need[customer_num], work)) {
      finish[customer_num] = true;
      /* release after the work is finished */
      for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        work[i] += allocation[customer_num][i];
      }

      /* find from the beginning */
      customer_num = 0;
      continue;
    }
    customer_num++;
  }

  /* step 3. If the loop above goes through all the customers but is still
    unfinished remaining, it's in an unsafe state. */
  for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
    if (!finish[i]) {
      return false;
    }
  }
  return true;
}

bool has_resources_to_work(int need[], int work[]) {
  for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
    if (need[i] > work[i]) {
      return false;
    }
  }
  return true;
}
