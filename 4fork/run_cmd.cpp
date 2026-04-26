#include <cstdlib>
#include <errno.h>
#include <iostream>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  pid_t pid = fork();
  if (pid == -1) {
    std::cerr << strerror(errno);
    return 1;
  }

  if (pid == 0) {
    if ((execvp(argv[1], &argv[1])) == -1) {
      std::cerr << strerror(errno) << "\n";
      _exit(127);
    }
  }

  if (pid != 0) {
    int status{};
    waitpid(pid, &status, 0);
    std::cout << "child " << pid << "\n";
    if (WIFEXITED(status)) {
      int code = WEXITSTATUS(status);
      std::cout << "exit " << code << "\n";
      return code;
    } else {
      if (WIFSIGNALED(status)) {
        std::cout << WTERMSIG(status) << "\n";
      }
    }
  }
}
