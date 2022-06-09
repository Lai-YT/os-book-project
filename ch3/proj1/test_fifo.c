#include <assert.h>
#include <stdio.h>

#include "fifo.h"

#define MAX_DATA_SIZE 80


int main(int argc, char const *argv[]) {
  queue_t queue;
  InitQueue(&queue, 10, MAX_DATA_SIZE);

  char *data[15] = {
    "a", "b", "c", "d", "e", "f", "g", "h",
    "i", "j", "k", "l", "m", "n", "o",
  };

  for (int i = 0; i < 15; i++) {
    while (!WriteQueue(&queue, data[i])) {
      char buf[MAX_DATA_SIZE];
      ReadQueue(&queue, buf);
    }
  }
  /* the queue is now full, so the write should fail */
  assert(IsFullQueue(&queue));
  assert(!WriteQueue(&queue, "-"));
  assert(CountQueueData(&queue) == 10);

  for (int i = 5; i < 15; i++) {
    char d[MAX_DATA_SIZE];
    ReadQueue(&queue, d);
    assert(strcmp(d, data[i]) == 0);
  }
  assert(IsEmptyQueue(&queue));

  FreeQueue(&queue);

  printf("FIFO Test ... (PASSED)\n");
  return 0;
}
