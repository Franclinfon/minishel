#include "minishell/exec_simple.h"

#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

static int status_from_wait(int wstatus)
{
    if (WIFEXITED(wstatus))
    {
        return WEXITSTATUS(wstatus);
    }

    if (WIFSIGNALED(wstatus))
    {
        return 128 + WTERMSIG(wstatus);
    }

    return 1;
}

int exec_simple(struct sh_ctx *ctx, char *const argv[])
{
    pid_t pid;
    int wstatus;

    if (ctx == NULL || argv == NULL || argv[0] == NULL)
    {
        return 0;
    }

    pid = fork();
    if (pid < 0)
    {
        ctx->last_status = 1;
        return -1;
    }

    if (pid == 0)
    {
        execvp(argv[0], argv);
        if (errno == ENOENT)
        {
            _exit(127);
        }
        _exit(126);
    }

    if (waitpid(pid, &wstatus, 0) < 0)
    {
        ctx->last_status = 1;
        return -1;
    }

    ctx->last_status = status_from_wait(wstatus);
    return 0;
}
