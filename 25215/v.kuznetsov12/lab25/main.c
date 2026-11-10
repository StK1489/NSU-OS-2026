#include <ctype.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

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

        unsigned char ch;
        ssize_t n;

        while ((n = read(fd[0], &ch, 1)) > 0) {
            ch = (unsigned char)toupper(ch);

            if (write(STDOUT_FILENO, &ch, 1) == -1) {
                perror("write");
                _exit(1);
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
    for (const char *p = text; *p != '\0'; ++p) {
        if (write(fd[1], p, 1) == -1) {
            perror("write");
            close(fd[1]);
            wait(NULL);
            return 1;
        }
    }

    close(fd[1]);

    if (wait(NULL) == -1) {
        perror("wait");
        return 1;
    }

    return 0;
}
