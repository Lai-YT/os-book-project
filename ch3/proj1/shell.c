#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>


#define MAX_LINE 80 /* The maximum length command */


bool IsChild(const pid_t pid);

bool IsParent(const pid_t pid);

bool IsToExit(const char *const command);


int main(void) {
  char *args[MAX_LINE / 2 + 1] = {0};  /* command line arguments */

  while (true) {
    /* prompt */
    printf("osh> ");
    fflush(stdout);

    char input[MAX_LINE] = {0};
    if (fgets(input, MAX_LINE, stdin) != NULL) {
      bool run_in_background = false;

      /* tokenize */
      int i = 0;
      args[i] = strtok(input, " \n");
      while (args[i] != NULL) {
        ++i;
        args[i] = strtok(NULL, " \n");
      }

      /* logout */
      if (IsToExit(args[0])) {
        break;
      }

      /* whether to run in background */
      if (strcmp(args[i - 1], "&") == 0) {
        run_in_background = true;
        args[--i] = NULL;
      }

      /* execute command */
      const pid_t pid = fork();
      if (pid < 0) { /* error occurred */
        fprintf(stderr, "Fork Failed");
        return 1;
      } else if (IsChild(pid)) {
        execvp(args[0], args);
        return 0;
      } else if (IsParent(pid)) {
        if (!run_in_background) {
          wait(NULL);
        }
      }
    }
  }

  return 0;
}


bool IsChild(const pid_t pid) {
  return pid == 0;
}

bool IsParent(const pid_t pid) {
  return pid > 0;
}

bool IsToExit(const char *const command) {
  return strcmp(command, "exit") == 0;
}
