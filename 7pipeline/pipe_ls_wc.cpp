#include <errno.h>
#include <iostream>
#include <sched.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  int fd[2]{};
  pipe(fd);
  pid_t pid = fork();
  if (pid == 0) {

    close(fd[0]);
    char *args[] = {"ls", "-l", nullptr};

    dup2(fd[1], STDOUT_FILENO);

    close(fd[1]);
    execvp("ls", args);
  }
  pid_t pid2 = fork();
  if (pid2 == 0) {
    close(fd[1]);
    char *args[] = {"wc", "-l", nullptr};

    dup2(fd[0], STDIN_FILENO);

    close(fd[0]);
    execvp("wc", args);
  }

  close(fd[0]);
  close(fd[1]);

  waitpid(pid, nullptr, 0);
  waitpid(pid2, nullptr, 0);

  return 0;
}
