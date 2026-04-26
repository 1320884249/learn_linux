#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <time.h>
#include <unistd.h>

static int set_nonblock(int fd) {
  int flags = fcntl(fd, F_GETFL);
  if (flags == -1)
    return -1;
  if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
    return -1;
  return 0;
}

int main() {
  if (set_nonblock(STDIN_FILENO) == -1) {
    std::cerr << "fcntl(O_NONBLOCK): " << std::strerror(errno) << "\n";
    return 1;
  }

  char buf[4096];
  long long sum = 0;
  int eagain_count = 0;

  while (true) {
    ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
    if (n > 0) {
      // 把读到的内容原样写到 stdout（处理部分写入）
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
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        // 不要刷屏：每 10 次提示一次
        if (++eagain_count % 10 == 0)
          std::cout << "EAGAIN\n";
      } else if (errno == EINTR) {
        // 被信号打断，重试
      } else {
        std::cerr << "read: " << std::strerror(errno) << "\n";
        return 1;
      }
    }

    // 让程序不会瞬间跑完，便于你交互输入；也避免忙等占 CPU
    timespec ts{0, 100 * 1000 * 1000}; // 100ms
    nanosleep(&ts, nullptr);
  }
}
