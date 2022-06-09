#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


/** A FIFO structure (circular queue) which stores char arrays. */
typedef struct {
  int head;
  int tail;
  int size;
  char **data;
} queue_t;

/**
 * @brief Initializes the queue as empty with `size`.
 * You must free the queue with `FreeQueue` to make sure the memories are
 * released properly.
 * @param queue  the queue to initialize
 * @param size   how many data can the queue contain,
 *               the actual size of queue will be 1 greater than this
 * @param data_size  how long can a single data be
 */
void InitQueue(queue_t *queue, int size, int data_size) {
  queue->head = 0;
  queue->tail = 0;  /* points to the next available space */
  queue->size = size + 1;  /* an unused space to separate full from empty */
  queue->data = malloc(sizeof(char *) * (size + 1));
  for (int i = 0; i < queue->size; i++) {
    queue->data[i] = malloc(sizeof(char) * data_size);
  }
}

/** Releases all data spaces and sets the `size` and pointers to 0. */
void FreeQueue(queue_t *queue) {
  for (int i = 0; i < queue->size; i++) {
    free(queue->data[i]);
  }
  free(queue->data);
  queue->size = 0;
  queue->head = 0;
  queue->tail = 0;
}


bool IsEmptyQueue(queue_t *queue) {
  return queue->head == queue->tail;
}


bool IsFullQueue(queue_t *queue) {
  return queue->head == (queue->tail + 1) % queue->size;
}

/** Reads the data out from the queue to the `buffer`; NULL if the queue is empty. */
void ReadQueue(queue_t *queue, char *buffer) {
  if (IsEmptyQueue(queue)) {
    buffer = NULL;
    return;
  }
  strcpy(buffer, queue->data[queue->head]);
  queue->head = (queue->head + 1) % queue->size;
}

/**
 * @brief Stores the `data` into the queue if the queue isn't full.
 * The `data` is copied.
 * @return true if the `data` is stored successfully; false if fails due to a full queue.
 */
bool WriteQueue(queue_t *queue, char *data) {
  if (IsFullQueue(queue)) {
    return false;
  }
  strcpy(queue->data[queue->tail], data);
  queue->tail = (queue->tail + 1) % queue->size;
  return true;
}


int CountQueueData(queue_t *queue) {
  return ((queue->tail + queue->size) - queue->head) % queue->size;
}
