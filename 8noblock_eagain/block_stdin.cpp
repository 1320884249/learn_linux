#include <cerrno>
#include <cstring>
#include <iostream>
#include <unistd.h>

int main() {
  char buf[4096];
  long long sum = 0;

  while (true) {
    ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
    if (n > 0) {
      ssize_t off = 0;
      while (off < n) {
        ssize_t m = write(STDOUT_FILENO, buf + off, n - off);
        if (m == -1) {
          if (errno == EINTR)
            continue;
          std::cerr << "write: " << std::strerror(errno) << "\n";
          return 1;
        }
        off += m;
        sum += m;
      }
    } else if (n == 0) {
      std::cout << "\nEOF\nsum=" << sum << "\n";
      return 0;
    } else { // n == -1
      if (errno == EINTR)
        continue;
      std::cerr << "read: " << std::strerror(errno) << "\n";
      return 1;
    }

    std::cout << "not here now\n";
  }
}
