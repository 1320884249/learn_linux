#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "usage: ./mycp <src> <dst>\n";
    return 1;
  }

  int fd = open(argv[1], O_RDONLY);
  if (fd == -1) {
    std::cerr << "open src failed: " << std::strerror(errno) << "\n";
    return 1;
  }

  int fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd2 == -1) {
    std::cerr << "open dst failed: " << std::strerror(errno) << "\n";
    close(fd);
    return 1;
  }

  char buffer[4096];
  while (true) {
    ssize_t n = read(fd, buffer, sizeof(buffer));
    if (n == 0)
      break; // EOF
    if (n < 0) {
      std::cerr << "read failed: " << std::strerror(errno) << "\n";
      close(fd);
      close(fd2);
      return 1;
    }

    ssize_t written = 0;
    while (written < n) {
      ssize_t m = write(fd2, buffer + written, n - written);
      if (m < 0) {
        std::cerr << "write failed: " << std::strerror(errno) << "\n";
        close(fd);
        close(fd2);
        return 1;
      }
      written += m;
    }
  }

  if (close(fd) == -1) {
    std::cerr << "close src failed: " << std::strerror(errno) << "\n";
    close(fd2);
    return 1;
  }
  if (close(fd2) == -1) {
    std::cerr << "close dst failed: " << std::strerror(errno) << "\n";
    return 1;
  }

  return 0;
}
