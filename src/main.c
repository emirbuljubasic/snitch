#define _GNU_SOURCE
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_SESSIONS 100

struct session_info {
  pid_t pid;
  char tty[32];
  char user[32];
  char command[32];
};

void trace_pid(pid_t pid) {
  if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) == -1) {
    perror("ptrace attach");
    exit(1);
  }

  waitpid(pid, NULL, 0);

  if (ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACESYSGOOD) == -1) {
    perror("ptrace options");
    exit(1);
  }

  int in_syscall = 0;
  long current_fd = -1;
  long current_buf = 0;
  long current_count = 0;

  while (1) {
    if (ptrace(PTRACE_SYSCALL, pid, NULL, NULL) == -1) {
      perror("ptrace syscall");
      break;
    }

    int status;
    if (waitpid(pid, &status, 0) == -1) {
      perror("waitpid");
      break;
    }

    if (WIFEXITED(status)) {
      break;
    }

    if (WIFSTOPPED(status)) {
      int sig = WSTOPSIG(status);
      if (sig == (SIGTRAP | 0x80)) {
        struct user_regs_struct regs;
        if (ptrace(PTRACE_GETREGS, pid, NULL, &regs) == -1) {
          perror("ptrace getregs");
          break;
        }

        if (in_syscall) {
          in_syscall = 0;
          if (regs.orig_rax == SYS_write &&
              (current_fd == 1 || current_fd == 2)) {
            long ret = regs.rax;
            if (ret > 0) {
              char data[4096];
              long to_read = ret;
              if (to_read > sizeof(data)) {
                to_read = sizeof(data);
              }

              for (long i = 0; i < to_read; i += sizeof(long)) {
                errno = 0;
                long word = ptrace(PTRACE_PEEKDATA, pid, current_buf + i, NULL);
                if (word == -1 && errno) {
                  break;
                }
                long remaining = to_read - i;
                long copy_size =
                    (remaining < sizeof(long)) ? remaining : sizeof(long);
                memcpy(data + i, &word, copy_size);
              }

              write(1, data, to_read);
            }
          }
        } else {
          in_syscall = 1;
          if (regs.orig_rax == SYS_write) {
            current_fd = regs.rdi;
            current_buf = regs.rsi;
            current_count = regs.rdx;
          } else {
            current_fd = -1;
          }
        }
      }
    }
  }

  ptrace(PTRACE_DETACH, pid, NULL, NULL);
}

void print_welcome() {
  printf("\n\033[1;34mSSH Session Monitor\033[0m\n");
  printf("-------------------------\n");
  printf("Available sessions:\n\n");
}

int main(int argc, char *argv[]) {
  if (argc == 2) {
    // Directly trace specified PID
    pid_t pid = atoi(argv[1]);
    trace_pid(pid);
    return 0;
  }

  // Show session selection menu
  struct session_info sessions[MAX_SESSIONS];
  int count = 0;

  FILE *fp = popen("ps -eo pid,tty,user,comm --no-headers", "r");
  if (!fp) {
    perror("popen");
    return 1;
  }

  char line[256];
  while (fgets(line, sizeof(line), fp) && count < MAX_SESSIONS) {
    char pid_str[16], tty[16], user[16], command[16];
    if (sscanf(line, "%s %s %s %s", pid_str, tty, user, command) != 4)
      continue;

    // Filter sessions: only pts terminals and specific shells
    if (strncmp(tty, "pts/", 4) != 0)
      continue;

    // Filter by shell commands
    int valid_shell = 0;
    const char *shells[] = {"bash", "zsh", "fish", "sh"};
    for (int i = 0; i < 4; i++) {
      if (strcmp(command, shells[i]) == 0) {
        valid_shell = 1;
        break;
      }
    }
    if (!valid_shell)
      continue;

    sessions[count].pid = atoi(pid_str);
    strncpy(sessions[count].tty, tty, sizeof(sessions[count].tty));
    strncpy(sessions[count].user, user, sizeof(sessions[count].user));
    strncpy(sessions[count].command, command, sizeof(sessions[count].command));
    count++;
  }
  pclose(fp);

  if (count == 0) {
    printf("No active terminal sessions found.\n");
    return 1;
  }

  print_welcome();
  for (int i = 0; i < count; i++) {
    printf("%d. PID: \033[1;32m%d\033[0m, TTY: \033[1;33m%s\033[0m, User: "
           "\033[1;36m%s\033[0m, Command: \033[1;35m%s\033[0m\n",
           i + 1, sessions[i].pid, sessions[i].tty, sessions[i].user,
           sessions[i].command);
  }

  printf("\n0. Exit\n");
  printf("\nSelect a session to monitor: ");

  int choice;
  if (scanf("%d", &choice) != 1) {
    printf("Invalid input\n");
    return 1;
  }

  if (choice == 0) {
    printf("Exiting...\n");
    return 0;
  }

  if (choice < 1 || choice > count) {
    printf("Invalid selection\n");
    return 1;
  }

  pid_t selected_pid = sessions[choice - 1].pid;
  printf("\nMonitoring session \033[1;32m%d\033[0m (TTY: "
         "\033[1;33m%s\033[0m)...\n",
         selected_pid, sessions[choice - 1].tty);
  printf("Press \033[1;31mCtrl+C\033[0m to stop monitoring\n\n");

  trace_pid(selected_pid);
  return 0;
}
