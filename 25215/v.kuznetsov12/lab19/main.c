#define _FILE_OFFSET_BITS 64
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define SIZE 4096

static int match(const char *pattern, const char *name) {
    const char *star = NULL;
    const char *saved = NULL;

    while (*name != '\0') {
        if (*pattern == '*') {
            star = pattern++;
            saved = name;
        } else if (*pattern == '?' || *pattern == *name) {
            pattern++;
            name++;
        } else if (star != NULL) {
            pattern = star + 1;
            saved++;
            name = saved;
        } else {
            return 0;
        }
    }

    while (*pattern == '*')
        pattern++;
    return *pattern == '\0';
}

int main(void) {
    char pattern[SIZE];
    int found = 0;

    printf("Enter pattern: ");
    fflush(stdout);
    if (fgets(pattern, sizeof pattern, stdin) == NULL) {
        fprintf(stderr, "Input error\n");
        return 1;
    }

    size_t len = strcspn(pattern, "\n");
    if (len == sizeof pattern - 1) {
        int c = getchar();
        if (c != '\n' && (c != EOF || ferror(stdin))) {
            fprintf(stderr, "Cannot read complete pattern\n");
            return 1;
        }
    }
    pattern[len] = '\0';
    if (strchr(pattern, '/') != NULL) {
        fprintf(stderr, "Pattern cannot contain '/'\n");
        return 1;
    }

    DIR *dir = opendir(".");
    if (dir == NULL) {
        perror("opendir");
        return 1;
    }

    while (1) {
        errno = 0;
        struct dirent *entry = readdir(dir);
        if (entry == NULL) {
            if (errno != 0) {
                perror("readdir");
                closedir(dir);
                return 1;
            }
            break;
        }

        const char *name = entry->d_name;
        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
            continue;
        if (match(pattern, name)) {
            puts(name);
            found = 1;
        }
    }

    if (closedir(dir) == -1) {
        perror("closedir");
        return 1;
    }
    if (!found)
        printf("No files matching '%s' found.\n", pattern);
    return 0;
}
