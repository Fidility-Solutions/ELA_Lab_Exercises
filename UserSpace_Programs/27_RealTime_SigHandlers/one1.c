#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/signalfd.h>

volatile sig_atomic_t allDone = 0;

void rt_signal_handler(int sig) {
    printf("Received real-time signal: %d\n", sig);
}

void siginfoHandler(int sig, siginfo_t *si, void *ucontext) {
    if (sig == SIGINT || sig == SIGTERM) {
        allDone = 1;
        return;
    }
    printf("Caught signal %d\n", sig);
    printf("si_signo=%d, si_code=%d (%s), ", si->si_signo, si->si_code,
           (si->si_code == SI_USER) ? "SI_USER" :
           (si->si_code == SI_QUEUE) ? "SI_QUEUE" : "other");
    printf("si_value=%d\n", si->si_value.sival_int);
    printf("si_pid=%ld, si_uid=%ld\n", (long) si->si_pid, (long) si->si_uid);
    sleep(1);
}

int main(int argc, char *argv[]) {
    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        printf("%s sig-num...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("%s: PID is %ld, UID is %ld\n", argv[0], (long) getpid(), (long) getuid());

    // Registering signal handler for most signals
    struct sigaction sa;
    sa.sa_sigaction = siginfoHandler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);

    for (int sig = 1; sig < NSIG; sig++) {
        if (sig != SIGTSTP && sig != SIGQUIT)
            sigaction(sig, &sa, NULL);
    }

    // Sending a real-time signal
    printf("Press Enter to send a real-time signal SIGRTMIN...\n");
    getchar();
    printf("Sending real-time signal SIGRTMIN...\n");
    union sigval sv;
    sv.sival_int = 42; 
    sigqueue(getpid(), SIGRTMIN, sv);

    // Registering signal handler for real-time signal SIGRTMIN
    sa.sa_handler = rt_signal_handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGRTMIN, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // Synchronous signal suspension
    printf("Press Ctrl+C to interrupt and suspend execution.\n");
    printf("Suspending execution until signal reception...\n");
    sigset_t initMask, oldMask;
    sigemptyset(&initMask);
    sigaddset(&initMask, SIGINT);
    if (sigsuspend(&initMask) == -1 && errno != EINTR) {
        perror("sigsuspend");
        exit(EXIT_FAILURE);
    }

    // Handling real-time signals using sigwaitinfo
    printf("Press Ctrl+C to interrupt and proceed to the next step.\n");
    printf("Waiting for a real-time signal using sigwaitinfo...\n");
    siginfo_t si;
    if (sigwaitinfo(&initMask, &si) == -1) {
        perror("sigwaitinfo");
        exit(EXIT_FAILURE);
    }
    printf("Received real-time signal: %d\n", si.si_signo);

    // Fetching signals via a file descriptor
    printf("Press Ctrl+C to interrupt and proceed to the next step.\n");
    printf("Fetching signals via a file descriptor...\n");
    int sfd = signalfd(-1, &initMask, 0);
    if (sfd == -1) {
        perror("signalfd");
        exit(EXIT_FAILURE);
    }
    struct signalfd_siginfo fdsi;
    printf("Press Ctrl+C to interrupt and terminate the program.\n");
    printf("Press Enter to continue fetching signals...\n");
    getchar();
    ssize_t s;
    while ((s = read(sfd, &fdsi, sizeof(struct signalfd_siginfo))) > 0) {
        printf("Received real-time signal via file descriptor: %d\n", fdsi.ssi_signo);
        if (fdsi.ssi_code == SI_QUEUE) {
            printf("ssi_pid = %d; ssi_int = %d\n", fdsi.ssi_pid, fdsi.ssi_int);
        }
        printf("Press Enter to continue fetching signals, or Ctrl+C to terminate the program.\n");
        getchar();
    }
    if (s == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    close(sfd);
    return 0;
}

