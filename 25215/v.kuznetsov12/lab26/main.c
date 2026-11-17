#include <stdio.h>

int main(void)
{
    const char message[] = "Hello, Solaris! AbCd 123.\n";
    FILE *channel = popen("tr '[a-z]' '[A-Z]'", "w");

    if (channel == NULL) {
        perror("popen");
        return 1;
    }

    size_t length = sizeof(message) - 1;
    size_t written = 0;
    size_t n;

    while (written < length && !ferror(channel) && (n = fwrite(message + written, 1, length - written, channel)) > 0) {
        written += n;
    }

    if (written != length || ferror(channel)) {
        perror("fwrite");
        pclose(channel);
        return 1;
    }

    int status = pclose(channel);

    if (status == -1) {
        perror("pclose");
        return 1;
    }

    return 0;
}