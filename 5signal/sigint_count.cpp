#include <cerrno>
#include <cstring>
#include <signal.h>
#include <unistd.h>

static volatile sig_atomic_t g_count = 0;

static void on_sigint(int) {
  ++g_count;
  const char msg[] = "SIGINT\n";
  write(STDOUT_FILENO, msg, sizeof(msg) - 1); // handler 里用 write
}

int main() {
  struct sigaction sa{};
  sa.sa_handler = on_sigint;
  sigemptyset(&sa.sa_mask); // handler 执行期间临时屏蔽的信号集合（这里为空）
  sa.sa_flags = 0;          // 可改成 SA_RESTART 做对比

  if (sigaction(SIGINT, &sa, nullptr) == -1) {
    const char *p = std::strerror(errno);
    write(STDERR_FILENO, p, std::strlen(p));
    write(STDERR_FILENO, "\n", 1);
    return 1;
  }

  while (true) {
    pause();
    write(STDOUT_FILENO, "tick\n", 4);
  }
  pause(); // 先放一个占位：等信号
  return 0;
}
