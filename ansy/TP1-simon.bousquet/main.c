#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/syscall.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <unistd.h>

#define TARGET_FD 3
#define TEST_FILE "test"
#define DUMMY_DATA "Some dummy data\n"

// Track syscall entry/exit state per syscall
static int in_openat = 0;
static int in_read = 0;
static int in_kill = 0;
static int in_sync = 0;
static int in_close = 0;

void trace_child(pid_t child) {
  int status;

  waitpid(child, &status, 0);
  ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_TRACESYSGOOD);

  while (1) {
    ptrace(PTRACE_SYSCALL, child, NULL, NULL);
    waitpid(child, &status, 0);

    if (WIFEXITED(status)) {
      _exit(WEXITSTATUS(status));
    }
    if (WIFSIGNALED(status)) {
      _exit(1);
    }

    // Syscall stop (0x80 flag set by PTRACE_O_TRACESYSGOOD)
    if (WIFSTOPPED(status) && (WSTOPSIG(status) & 0x80)) {
      long syscall_nr = ptrace(PTRACE_PEEKUSER, child, 8 * ORIG_RAX, NULL);

      // openat -> return 3
      if (syscall_nr == SYS_openat) {
        if (in_openat) {
          ptrace(PTRACE_POKEUSER, child, 8 * RAX, 3);
        }
        in_openat = !in_openat;
      }
      // read -> return 493
      else if (syscall_nr == SYS_read) {
        if (in_read) {
          ptrace(PTRACE_POKEUSER, child, 8 * RAX, 493);
        }
        in_read = !in_read;
      }
      // kill -> return 1
      else if (syscall_nr == SYS_kill) {
        if (in_kill) {
          ptrace(PTRACE_POKEUSER, child, 8 * RAX, 1);
        }
        in_kill = !in_kill;
      }
      // sync -> return 0
      else if (syscall_nr == SYS_sync) {
        if (in_sync) {
          ptrace(PTRACE_POKEUSER, child, 8 * RAX, 0);
        }
        in_sync = !in_sync;
      }
      // close -> return 0
      else if (syscall_nr == SYS_close) {
        if (in_close) {
          ptrace(PTRACE_POKEUSER, child, 8 * RAX, 0);
        }
        in_close = !in_close;
      }
    } else if (WIFSTOPPED(status)) {
      // Deliver signals except SIGSEGV
      int sig = WSTOPSIG(status);
      if (sig != SIGSEGV) {
        ptrace(PTRACE_SYSCALL, child, NULL, sig);
        continue;
      }
    }
  }
}

int main(int argc, char *argv[], char *envp[]) {
  int fd;
  struct utsname uts;
  char secret_filename[512];
  struct passwd *pw;
  const char *username;

  // Get current username (equivalent to whoami)
  pw = getpwuid(getuid());
  if (!pw) {
    perror("getpwuid");
    return 1;
  }
  username = pw->pw_name;

  // Get uname info
  if (uname(&uts) < 0) {
    perror("uname");
    return 1;
  }

  // Create filename: {release}-{nodename}-100.{username}-megasecretfile
  snprintf(secret_filename, sizeof(secret_filename),
           "%s-%s-100.%s-megasecretfile", uts.release, uts.nodename, username);

  // Create the secret file with exactly 493 bytes
  fd = open(secret_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd < 0) {
    perror("open (create secret file)");
    return 1;
  }
  char filler[493];
  memset(filler, 'A', 493);
  if (write(fd, filler, 493) != 493) {
    perror("write (secret file)");
    close(fd);
    return 1;
  }
  close(fd);
  printf("Created: %s (493 bytes)\n", secret_filename);

  // Create the test file with dummy data
  fd = open(TEST_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd < 0) {
    perror("open (create test file)");
    return 1;
  }
  if (write(fd, DUMMY_DATA, strlen(DUMMY_DATA)) < 0) {
    perror("write");
    close(fd);
    return 1;
  }
  close(fd);

  // Open the test file for reading
  fd = open(TEST_FILE, O_RDONLY);
  if (fd < 0) {
    perror("open (read test file)");
    return 1;
  }

  // Ensure fd 3 points to test file
  if (fd != TARGET_FD) {
    if (dup2(fd, TARGET_FD) < 0) {
      perror("dup2");
      close(fd);
      return 1;
    }
    close(fd);
  }

  // Fork and ptrace to intercept syscalls
  pid_t child = fork();
  if (child < 0) {
    perror("fork");
    return 1;
  }

  if (child == 0) {
    // Child: request to be traced, then exec
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    signal(SIGSEGV, SIG_IGN);
    char *args[] = {"./straceme", NULL};
    execve("./straceme", args, envp);
    perror("execve");
    _exit(1);
  }

  // Parent: trace the child
  trace_child(child);
  return 0;
}
