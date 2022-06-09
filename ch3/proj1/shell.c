#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "fifo.h"

/**
 * I use a randomly accessible circular queue to inplement the history feature.
 * The part to be careful is the manipulation on `head` and `tail` indices,
 * since they have "modulo" relation.
 */

#define MAX_LINE 80 /* The maximum length command */
#define HISTORY_SIZE 10


bool IsChildProcess(const pid_t pid);

bool IsParentProcess(const pid_t pid);

bool IsExitCommand(const char *const command);

void RemoveTrailingLineBreakFromLine(char *line /* in-out parameter */);

/**
 * @brief Pulls the index back to 0 when the `move` makes it out of the range of the queue.
 * @param queue  the queue of which we are moving the index about
 * @param index  the base index to perform movement on
 * @param move   the number to add to `index`; can be negative for moving backward,
 *               but its absolute value shouldn't be greater than the size of the `queue`
 * @return int   the index after moved
 */
int MoveIndexOfCircularQueue(const queue_t *const queue, const int index, const int move);


int main(void) {
  char *args[MAX_LINE / 2 + 1] = {0};  /* command line arguments */
  queue_t history;
  InitQueue(&history, HISTORY_SIZE, MAX_LINE);

  while (true) {
    /* prompt */
    printf("osh> ");
    fflush(stdout);

    char input[MAX_LINE] = {0};
    if (fgets(input, MAX_LINE, stdin) == NULL) {
      continue;
    }
    RemoveTrailingLineBreakFromLine(input);

    if (strlen(input) == 2 && strcmp(input, "!!") == 0) {
      if (IsEmptyQueue(&history)) {
        printf("No commands in history\n");
        continue;
      }
      strcpy(input, history.data[MoveIndexOfCircularQueue(&history, history.tail, -1)]);
      printf("%s\n", input);
    } else if (input[0] == '!') {
      int history_number = atoi(input + 1);
      if (history_number <= 0 || history_number > CountQueueData(&history)) {
        printf("No such command in history.\n");
        continue;
      }
      strcpy(input, history.data[MoveIndexOfCircularQueue(&history, history.tail, -history_number)]);
      printf("%s\n", input);
    }
    /* `!!` and `!` commands escape from the history */

    /* add new history */
    while (!WriteQueue(&history, input)) {
      char buf[MAX_LINE];
      ReadQueue(&history, buf);
    }

    bool run_in_background = false;

    /* tokenize */
    int i = 0;
    args[i] = strtok(input, " \n");
    while (args[i] != NULL) {
      args[++i] = strtok(NULL, " \n");
    }

    /* logout */
    if (IsExitCommand(args[0])) {
      break;
    }

    /* show history */
    if (strcmp(args[0], "history") == 0) {
      if (IsEmptyQueue(&history)) {
        printf("No commands in history\n");
      } else {
        int data_count = CountQueueData(&history);
        for (int i = history.head; i != history.tail; i = MoveIndexOfCircularQueue(&history, i, 1)) {
          printf("%d %s\n", data_count--, history.data[i]);
        }
      }
      continue;
    }

    /* whether to run in background */
    if (strcmp(args[i - 1], "&") == 0) {
      run_in_background = true;
      args[--i] = NULL;  /* remove `&` */
    }

    /* execute command */
    const pid_t pid = fork();
    if (pid < 0) {  /* error occurred */
      fprintf(stderr, "Fork Failed");
      return 1;
    } else if (IsChildProcess(pid)) {
      execvp(args[0], args);
    } else if (IsParentProcess(pid)) {
      if (!run_in_background) {
        wait(NULL);
      }
    }
  }

  return 0;
}


bool IsChildProcess(const pid_t pid) {
  return pid == 0;
}


bool IsParentProcess(const pid_t pid) {
  return pid > 0;
}


bool IsExitCommand(const char *const command) {
  return strcmp(command, "exit") == 0;
}


void RemoveTrailingLineBreakFromLine(char *line) {
  int len = strlen(line);
  /* fill '\0' until the last non-new-line character */
  while (len && line[len - 1] == '\n') {
    line[--len] = '\0';
  }
}


int MoveIndexOfCircularQueue(const queue_t *const queue, const int index, const int move) {
  return ((index + queue->size) + move) % queue->size;
}
