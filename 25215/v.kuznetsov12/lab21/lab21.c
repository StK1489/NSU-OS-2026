#include <stdio.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t count = 0;
volatile sig_atomic_t quitflag = 0;

void handle_int(int sig) {
    (void)sig;
    write(1, "\a", 1);
    count++;
}

void handle_quit(int sig) {
    (void)sig;
    quitflag = 1;
}

int main(void) {
    sigset(SIGINT, handle_int);
    sigset(SIGQUIT, handle_quit);

    while (!quitflag)
        sleep(1);

    printf("\nNumber of beeps: %d\n", (int)count);
    return 0;
}