#include <errno.h>
#include <iostream>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

int main() {
  int epfd = epoll_create(2);
  epoll_event ev{};
  ev.events = EPOLLIN;
  ev.data.fd = STDIN_FILENO;

  epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev);

  epoll_event events[8];

  char buf[4096]{};
  long long sum = 0;

  while (true) {
    int nready = epoll_wait(epfd, events, 8, 3000);
    if (nready == -1) {
      if (errno == EINTR)
        continue;
      std::cerr << "epoll wait: " << strerror(errno) << "\n";
      close(epfd);
      return 1;
    }

    if (nready == 0) {
      std::cout << "timeout\n";
      continue;
    }

    for (int i{}; i < nready; i++) {
      int fd = events[i].data.fd;
      if (events[i].events & (EPOLLERR | EPOLLHUP)) {
        // 对stdin，通常接下来read会得到0或错误
      }

      if (fd == STDIN_FILENO && (events[i].events & EPOLLIN)) {
        ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
        if (n > 0) {
          ssize_t off = 0;
          while (off < n) {
            ssize_t m = write(STDOUT_FILENO, buf + off, n - off);

            if (m == -1) {
              if (errno == EINTR)
                continue;
              std::cerr << "write: " << strerror(errno) << "\n";
              close(epfd);
              return 1;
            }

            off += m;
            sum += m;
          }
        } else if (n == 0) {
          std::cout << "\nEOF\nsum=" << sum << "\n";
          close(epfd);
          return 0;
        } else {
          if (errno == EINTR)
            continue;
          std::cerr << "read: " << strerror(errno) << "\n";
          close(epfd);
          return 1;
        }
      }
    }
  }
}
