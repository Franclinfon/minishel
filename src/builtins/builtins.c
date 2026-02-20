#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "minishell/builtins.h"

static int builtin_echo(char *const argv[])
{
    int i;
    int no_nl;

    i = 1;
    no_nl = 0;
    if (argv[1] != NULL && strcmp(argv[1], "-n") == 0)
    {
        no_nl = 1;
        i = 2;
    }

    while (argv[i] != NULL)
    {
        if (i != (no_nl ? 2 : 1))
        {
            write(1, " ", 1);
        }
        write(1, argv[i], strlen(argv[i]));
        i++;
    }

    if (!no_nl)
    {
        write(1, "\n", 1);
    }

    return 0;
}

static int builtin_cd(char *const argv[])
{
    if (argv[1] == NULL)
    {
        /* sujet: pas obligé de gérer cd sans args -> on choisit erreur */
        return 1;
    }

    if (chdir(argv[1]) != 0)
    {
        return 1;
    }

    return 0;
}

static int parse_signal_opt(const char *s, int *out_sig)
{
    long v;
    char *end;

    if (s == NULL || s[0] != '-' || s[1] == '\0')
    {
        return -1;
    }

    v = strtol(s + 1, &end, 10);
    if (*end != '\0' || v <= 0 || v > 2147483647L)
    {
        return -1;
    }

    *out_sig = (int)v;
    return 0;
}

static int builtin_kill(char *const argv[])
{
    int sig;
    int i;

    sig = SIGTERM;
    i = 1;

    if (argv[i] != NULL && argv[i][0] == '-')
    {
        if (parse_signal_opt(argv[i], &sig) != 0)
        {
            return 1;
        }
        i++;
    }

    if (argv[i] == NULL)
    {
        return 1;
    }

    while (argv[i] != NULL)
    {
        long pid;
        char *end;

        pid = strtol(argv[i], &end, 10);
        if (*end != '\0' || pid <= 0)
        {
            return 1;
        }

        if (kill((pid_t)pid, sig) != 0)
        {
            return 1;
        }

        i++;
    }

    return 0;
}

int builtin_is(const char *name)
{
    if (name == NULL)
    {
        return 0;
    }

    return strcmp(name, "echo") == 0 || strcmp(name, "cd") == 0 ||
           strcmp(name, "exit") == 0 || strcmp(name, "kill") == 0;
}

int builtin_exec(struct sh_ctx *ctx, char *const argv[])
{
    int code;

    if (argv == NULL || argv[0] == NULL)
    {
        return 1;
    }

    if (strcmp(argv[0], "echo") == 0)
    {
        code = builtin_echo(argv);
        ctx->last_status = code;
        return 0;
    }

    if (strcmp(argv[0], "cd") == 0)
    {
        code = builtin_cd(argv);
        ctx->last_status = code;
        return 0;
    }

    if (strcmp(argv[0], "kill") == 0)
    {
        code = builtin_kill(argv);
        ctx->last_status = code;
        return 0;
    }

    if (strcmp(argv[0], "exit") == 0)
    {
        code = 0;
        if (argv[1] != NULL)
        {
            code = atoi(argv[1]);
        }
        ctx->last_status = code;
        _exit(code);
    }

    ctx->last_status = 1;
    return -1;
}
