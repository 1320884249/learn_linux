#include <errno.h>
#include <iostream>
#include <poll.h>
#include <string.h>
#include <sys/poll.h>
#include <unistd.h>

int main() {
  struct pollfd my_fds{};
  my_fds.fd = STDIN_FILENO;
  my_fds.events = POLLIN;

  char buf[4096]{};
  int sum{};
  while (true) {

    int n = poll(&my_fds, 1, 3000);

    if (n == -1) {
      if (errno == EINTR)
        continue;
      std::cerr << "poll: " << strerror(errno) << "\n";
      return 1;
    }
    if (n == 0) {
      std::cout << "timeout" << std::endl;
      continue;
    }

    if (my_fds.revents & POLLERR) {
      std::cerr << "POLLERR\n";
      return 1;
    }
    if (my_fds.revents & POLLHUP) {
      // 另一端关闭，通常接下来 read 会返回0
      ssize_t m = read(STDIN_FILENO, buf, sizeof(buf));
      if (m == 0) {
        std::cout << "EOF\nsum=" << sum << "\n";
        return 0;
      }
      if (m > 0) {
        ssize_t off{};
        while (off < n) {
          ssize_t p = write(STDOUT_FILENO, buf + off, m - off);
          if (p == -1) {
            if (errno == EINTR)
              continue;
            std::cerr << "write: " << strerror(errno) << "\n";
            return 1;
          }
          off += p;
          sum += p;
        }
        continue;
      }
      if (errno != EINTR) {
        std::cerr << "read: " << strerror(errno) << "\n";
        return 1;
      }
      continue;
    }

    if (my_fds.revents & POLLIN) {
      ssize_t m = read(STDIN_FILENO, buf, sizeof(buf));
      if (m > 0) {
        ssize_t off{};
        while (off < n) {
          ssize_t p = write(STDOUT_FILENO, buf + off, m - off);
          if (p == -1) {
            if (errno == EINTR)
              continue;
            std::cerr << "write: " << strerror(errno) << "\n";
            return 1;
          }
          off += m;
          sum += m;
        }

      }

      else if (m == 0) {
        std::cout << "\nEOF\nsum=" << sum << "\n";
        return 0;
      } else {
        if (errno == EINTR)
          continue;
        std::cerr << "read: " << strerror(errno) << "\n";
        return 1;
      }
    }
  }
}
