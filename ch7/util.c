#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "util.h"

/* the available amount of each resource */
int available[NUMBER_OF_RESOURCES];

/* the maximum demand of each customer */
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the amount currently allocated to each customer */
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the remaining need of each customer */
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

pthread_mutex_t resource_mutex;
pthread_mutex_t rand_mutex;

void read_available(int argc, const char *argv[]) {
  if (argc != 4) {
    printf("Usage: bank [AVAILABLE_1] [AVAILABLE_2] [AVAILABLE_3]\n");
    exit(EXIT_FAILURE);
  }
  ++argv; /* ignore the program name itself */
  for (int i = 0; i < 3; i++) {
    const int num = atoi(argv[i]);
    if (num < 0) {
      printf("Error: the number of available resources can't be negative.\n");
      exit(EXIT_FAILURE);
    }
    available[i] = num;
  }
}

void *enter_bank(void *customer_num_) {
  const int customer_num = *((int *)customer_num_);

  for (int i = 0; i < NUMBER_OF_REQUESTS; i++) {
    pthread_mutex_lock(&resource_mutex);
    make_request(customer_num);
    pthread_mutex_unlock(&resource_mutex);

    pthread_mutex_lock(&resource_mutex);
    make_release(customer_num);
    pthread_mutex_unlock(&resource_mutex);
  }

  pthread_exit(NULL);
}

void make_request(int customer_num) {
  int *request = gen_random_resources(need[customer_num]);
  enum Status status = request_resources(customer_num, request);

  char examination_res[8] = {0}; /* either DENIED or GRANTED */
  if (status == FAILURE) {
    sprintf(examination_res, "DENIED");
  } else {
    sprintf(examination_res, "GRANTED");
  }
  printf("[REQUEST %s ON CUSTOMER %d]\n", examination_res, customer_num);
  print_request(request);

  free(request);

  print_state();
  printf("####\n");
}

void make_release(int customer_num) {
  int *release = gen_random_resources(allocation[customer_num]);
  release_resources(customer_num, release);

  printf("[RELEASE BY CUSTOMER %d]\n", customer_num);
  print_release(release);

  free(release);

  print_state();
  printf("####\n");
}

void grant_request(int customer_num, int request[]);

enum Status request_resources(int customer_num, int request[]) {
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

enum Status release_resources(int customer_num, int release[]) {
  for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
    allocation[customer_num][i] -= release[i];
    available[i] += release[i];
    need[customer_num][i] += release[i];
  }
  return SUCCESS;
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

void init_state(void) {
  for (int customer_num = 0; customer_num < NUMBER_OF_CUSTOMERS;
       customer_num++) {
    int *max = gen_random_resources(available);
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
      maximum[customer_num][i] = max[i];
      need[customer_num][i] = max[i];
      allocation[customer_num][i] = 0; /* no resource allocated yet */
    }
    free(max);
  }
}

void print_state(void) {
  printf("The current state is:\n"
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
  int work[NUMBER_OF_RESOURCES] = {0};
  for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
    work[i] = available[i];
  }
  bool finish[NUMBER_OF_CUSTOMERS] = {0};
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
