// inspect_sparse.cpp
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

int main() {
  const char *path = "sparse.bin";

  int fd = open(path, O_RDONLY);
  if (fd == -1) {
    std::cerr << "open failed: " << std::strerror(errno) << "\n";
    return 1;
  }

  char buf[3];

  ssize_t n = read(fd, buf, 3);
  if (n != 3) {
    std::cerr << "read head failed: " << std::strerror(errno) << "\n";
    close(fd);
    return 1;
  }
  if (write(STDOUT_FILENO, buf, 3) != 3 || write(STDOUT_FILENO, "\n", 1) != 1) {
    std::cerr << "stdout write failed: " << std::strerror(errno) << "\n";
    close(fd);
    return 1;
  }

  if (lseek(fd, -3, SEEK_END) == (off_t)-1) {
    std::cerr << "lseek end failed: " << std::strerror(errno) << "\n";
    close(fd);
    return 1;
  }

  n = read(fd, buf, 3);
  if (n != 3) {
    std::cerr << "read tail failed: " << std::strerror(errno) << "\n";
    close(fd);
    return 1;
  }
  if (write(STDOUT_FILENO, buf, 3) != 3 || write(STDOUT_FILENO, "\n", 1) != 1) {
    std::cerr << "stdout write failed: " << std::strerror(errno) << "\n";
    close(fd);
    return 1;
  }

  if (close(fd) == -1) {
    std::cerr << "close failed: " << std::strerror(errno) << "\n";
    return 1;
  }

  return 0;
}
