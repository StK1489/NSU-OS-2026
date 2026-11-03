#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        return 1;
    }

    pid_t child = fork();

    if (child == -1) {
        perror("fork");
        return 1;
    }

    if (child == 0) {
        execlp("cat", "cat", argv[1], (char *)NULL);
        perror("execlp");
        _exit(1);
    }

    if (waitpid(child, NULL, 0) == -1) {
        perror("waitpid");
        return 1;
    }

    printf("\nParent: child has finished.\n");
    return 0;
}