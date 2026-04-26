// sigsuspend_demo.cpp
#include <cerrno>
#include <cstring>
#include <signal.h>
#include <unistd.h>

static volatile sig_atomic_t g_got_int = 0;

static void on_sigint(int) {
  g_got_int = 1;
  const char msg[] = "SIGINT\n";
  write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

static void die(const char *what) {
  const char *e = std::strerror(errno);
  write(STDERR_FILENO, what, std::strlen(what));
  write(STDERR_FILENO, ": ", 2);
  write(STDERR_FILENO, e, std::strlen(e));
  write(STDERR_FILENO, "\n", 1);
  _exit(1);
}

int main() {
  // 1) install handler
  struct sigaction sa;
  sa.sa_handler = on_sigint;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  if (sigaction(SIGINT, &sa, nullptr) == -1)
    die("sigaction");

  const char prompt[] = "press Ctrl+C to continue\n";
  if (write(STDOUT_FILENO, prompt, sizeof(prompt) - 1) == -1)
    die("write");

  // 2) block SIGINT (avoid race)
  sigset_t block_set, oldmask;
  sigemptyset(&block_set);
  sigaddset(&block_set, SIGINT);

  if (sigprocmask(SIG_BLOCK, &block_set, &oldmask) == -1)
    die("sigprocmask");

  // 3) wait atomically using sigsuspend
  //    suspend_mask = oldmask with SIGINT unblocked
  sigset_t suspend_mask = oldmask;
  sigdelset(&suspend_mask, SIGINT);

  while (!g_got_int) {
    // returns -1 with errno=EINTR after a signal is handled (normal)
    if (sigsuspend(&suspend_mask) == -1 && errno != EINTR) {
      die("sigsuspend");
    }
  }

  // 4) restore previous mask
  if (sigprocmask(SIG_SETMASK, &oldmask, nullptr) == -1)
    die("sigprocmask restore");

  const char done[] = "done\n";
  if (write(STDOUT_FILENO, done, sizeof(done) - 1) == -1)
    die("write done");

  return 0;
}
