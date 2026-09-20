#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

extern char **environ;

typedef struct {
    int code;
    char *value;
    int bad;
} opt_record;

static void act_ids(void) {
    printf("Real UID = %d, Real GID = %d\n",
           (int)getuid(), (int)getgid());
    printf("Effective UID = %d, Effective GID = %d\n",
           (int)geteuid(), (int)getegid());
}

static void act_leader(void) {
    if (setpgid(0, 0) == -1)
        perror("setpgid");
    else
        printf("Process is now a group leader, PGID = %d\n", getpgrp());
}

static void act_proc(void) {
    printf("PID = %d\n", getpid());
    printf("PPID = %d\n", getppid());
    printf("PGID = %d\n", getpgrp());
}

static void act_ulimit_show(void) {
    struct rlimit rl;
    if (getrlimit(RLIMIT_FSIZE, &rl) == -1) {
        perror("getrlimit");
        return;
    }
    printf("ulimit: soft = %ld, hard = %ld\n",
           (long)rl.rlim_cur, (long)rl.rlim_max);
}

static void act_ulimit_set(const char *s) {
    char *end;
    long v;
    struct rlimit rl;

    errno = 0;
    v = strtol(s, &end, 10);
    if (errno == ERANGE) {
        fprintf(stderr, "Number out of range for -U: %s\n", s);
        return;
    }
    if (errno != 0 || *end != '\0' || end == s) {
        fprintf(stderr, "Not a number for -U: %s\n", s);
        return;
    }
    if (getrlimit(RLIMIT_FSIZE, &rl) == -1) {
        perror("getrlimit");
        return;
    }
    rl.rlim_cur = v;
    if (setrlimit(RLIMIT_FSIZE, &rl) == -1)
        perror("setrlimit");
    else
        printf("ulimit changed to %ld\n", v);
}

static void act_core_show(void) {
    struct rlimit rl;
    if (getrlimit(RLIMIT_CORE, &rl) == -1) {
        perror("getrlimit");
        return;
    }
    printf("core size: soft = %ld, hard = %ld\n",
           (long)rl.rlim_cur, (long)rl.rlim_max);
}

static void act_core_set(const char *s) {
    char *end;
    long v;
    struct rlimit rl;

    errno = 0;
    v = strtol(s, &end, 10);
    if (errno == ERANGE) {
        fprintf(stderr, "Number out of range for -C: %s\n", s);
        return;
    }
    if (errno != 0 || *end != '\0' || end == s) {
        fprintf(stderr, "Not a number for -C: %s\n", s);
        return;
    }
    if (getrlimit(RLIMIT_CORE, &rl) == -1) {
        perror("getrlimit");
        return;
    }
    rl.rlim_cur = v;
    if (setrlimit(RLIMIT_CORE, &rl) == -1)
        perror("setrlimit");
    else
        printf("core size changed to %ld\n", v);
}

static void act_cwd(void) {
    char buf[4096];
    if (getcwd(buf, sizeof(buf)) != NULL)
        printf("CWD = %s\n", buf);
    else
        perror("getcwd");
}

static void act_env(void) {
    char **p;
    for (p = environ; *p != NULL; p++)
        printf("%s\n", *p);
}

static void act_env_set(const char *s) {
    char *tmp;
    char *eq;

    tmp = strdup(s);
    if (tmp == NULL) {
        perror("strdup");
        return;
    }
    eq = strchr(tmp, '=');
    if (eq == NULL) {
        fprintf(stderr, "-V expects name=value, got: %s\n", s);
        free(tmp);
        return;
    }
    *eq = '\0';
    if (setenv(tmp, eq + 1, 1) == -1)
        perror("setenv");
    else
        printf("Set %s=%s\n", tmp, eq + 1);
    free(tmp);
}

static void handle(const opt_record *o) {
    switch (o->code) {
        case 'i': act_ids();                break;
        case 's': act_leader();             break;
        case 'p': act_proc();               break;
        case 'u': act_ulimit_show();        break;
        case 'U': act_ulimit_set(o->value); break;
        case 'c': act_core_show();          break;
        case 'C': act_core_set(o->value);   break;
        case 'd': act_cwd();                break;
        case 'v': act_env();                break;
        case 'V': act_env_set(o->value);    break;
        case '?': fprintf(stderr, "Unknown option: -%c\n", o->bad);    break;
        case ':': fprintf(stderr, "Missing argument for -%c\n", o->bad); break;
    }
}

int main(int argc, char *argv[]) {
    opt_record opts[128];
    int n = 0;
    int c;

    while ((c = getopt(argc, argv, ":ispuU:cC:dvV:")) != -1) {
        if (n >= 128) {
            fprintf(stderr, "Too many options\n");
            return 1;
        }
        opts[n].code  = c;
        opts[n].value = optarg;
        opts[n].bad   = optopt;
        n++;
    }

    while (n-- > 0)
        handle(&opts[n]);

    return 0;
}