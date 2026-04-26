#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>

int main() {
  int fd{};
  fd = open("out.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd == -1) {
    std::cerr << strerror(errno);
    return 1;
  }

  if (dup2(fd, STDOUT_FILENO) == -1) {
    std::cerr << strerror(errno);
    close(fd);
    return -1;
  }

  close(fd);
  std::cout << "hello dup2\n";

  if ((write(STDOUT_FILENO, "syscall line\n", 13)) == -1) {
    std::cerr << strerror(errno);
    close(fd);
    return 1;
  }

  return 0;
}
