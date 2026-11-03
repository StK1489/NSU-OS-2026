#include <stdio.h>
#include <unistd.h>

extern char **environ;

int execvpe(const char *file, char *const argv[], char *const envp[])
{
    char **saved_environ = environ;

    environ = (char **)envp;
    execvp(file, argv);

    environ = saved_environ;
    return -1;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Please specify a command to run.\n");
        return 1;
    }

    char *envp[] = {"PATH=/usr/bin:/bin",
         "TEST=hello",
          NULL
        };

    execvpe(argv[1], argv + 1, envp);
    perror("execvpe");
    return 1;
}