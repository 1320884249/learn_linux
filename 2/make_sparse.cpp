// make_sparse.cpp
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

int main() {
  const char *path = "sparse.bin";

  int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd == -1) {
    std::cerr << "open failed: " << std::strerror(errno) << "\n";
    return 1;
  }

  if (write(fd, "ABC", 3) != 3) {
    std::cerr << "write head failed: " << std::strerror(errno) << "\n";
    close(fd);
    return 1;
  }

  off_t off = lseek(fd, 10 * 1024 * 1024, SEEK_CUR);
  if (off == (off_t)-1) {
    std::cerr << "lseek failed: " << std::strerror(errno) << "\n";
    close(fd);
    return 1;
  }

  if (write(fd, "XYZ", 3) != 3) {
    std::cerr << "write tail failed: " << std::strerror(errno) << "\n";
    close(fd);
    return 1;
  }

  if (close(fd) == -1) {
    std::cerr << "close failed: " << std::strerror(errno) << "\n";
    return 1;
  }

  return 0;
}
