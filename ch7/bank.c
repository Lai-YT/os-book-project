#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#include "util.h"

int main(int argc, const char *argv[]) {
  srand(time(NULL));

  pthread_mutex_init(&resource_mutex, NULL);
  pthread_mutex_init(&rand_mutex, NULL);

  read_available(argc, argv);
  init_state();

  pthread_t customers[NUMBER_OF_CUSTOMERS];
  /* NOTE: customer_nums are passed as pointers, so we have to keep them
    untouched in another place. I've tried to passed the one incrementing
    with the loop, which makes all the threads use the same customer_num and
    breaks.
  */
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

  print_state();

  pthread_mutex_destroy(&resource_mutex);
  pthread_mutex_destroy(&rand_mutex);
  return 0;
}
