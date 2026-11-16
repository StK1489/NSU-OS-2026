#include <ctype.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(void)
{
    int fd[2];

    if (pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t child = fork();

    if (child == -1) {
        perror("fork");
        close(fd[0]);
        close(fd[1]);
        return 1;
    }

    if (child == 0) {
        close(fd[1]);

        unsigned char buffer[BUFFER_SIZE];
        ssize_t n;

        while ((n = read(fd[0], buffer, sizeof(buffer))) > 0) {
            for (ssize_t i = 0; i < n; ++i) {
                buffer[i] = (unsigned char)toupper(buffer[i]);
            }

            size_t written = 0;
            while (written < (size_t)n) {
                ssize_t result = write(STDOUT_FILENO, buffer + written,
                                       (size_t)n - written);
                if (result == -1) {
                    perror("write");
                    _exit(1);
                }
                written += (size_t)result;
            }
        }

        if (n == -1) {
            perror("read");
            _exit(1);
        }

        close(fd[0]);
        _exit(0);
    }

    close(fd[0]);

    const char text[] = "Hello, Solaris! AbCd 123.\n";
    size_t written = 0;
    while (written < sizeof(text) - 1) {
        ssize_t result = write(fd[1], text + written,
                               sizeof(text) - 1 - written);
        if (result == -1) {
            perror("write");
            close(fd[1]);
            wait(NULL);
            return 1;
        }
        written += (size_t)result;
    }

    close(fd[1]);

    if (wait(NULL) == -1) {
        perror("wait");
        return 1;
    }

    return 0;
}