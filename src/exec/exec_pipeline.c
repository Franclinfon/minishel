#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

#include "minishell/exec_command.h"
#include "minishell/exec_pipeline.h"

static int status_from_wait(int wstatus)
{
    if (WIFEXITED(wstatus))
        return WEXITSTATUS(wstatus);
    if (WIFSIGNALED(wstatus))
        return 128 + WTERMSIG(wstatus);
    return 1;
}

int exec_pipeline(struct sh_ctx *ctx, const struct ast_pipeline *pl)
{
    size_t i;
    int prev_fd;
    int pipefd[2];
    pid_t *pids;
    int wstatus;

    if (ctx == NULL || pl == NULL || pl->cmds.len == 0)
        return 0;

    pids = calloc(pl->cmds.len, sizeof(pid_t));
    if (!pids)
        return -1;

    prev_fd = -1;

    for (i = 0; i < pl->cmds.len; i++)
    {
        if (i < pl->cmds.len - 1 && pipe(pipefd) < 0)
            return -1;

        pids[i] = fork();
        if (pids[i] < 0)
            return -1;

        if (pids[i] == 0)
        {
            if (prev_fd != -1)
            {
                dup2(prev_fd, 0);
                close(prev_fd);
            }

            if (i < pl->cmds.len - 1)
            {
                close(pipefd[0]);
                dup2(pipefd[1], 1);
                close(pipefd[1]);
            }

            exec_command_mode(ctx, &pl->cmds.items[i]->data.command, 0);
            _exit(ctx->last_status);
        }

        if (prev_fd != -1)
            close(prev_fd);

        if (i < pl->cmds.len - 1)
        {
            close(pipefd[1]);
            prev_fd = pipefd[0];
        }
    }

    for (i = 0; i < pl->cmds.len; i++)
    {
        waitpid(pids[i], &wstatus, 0);
        if (i == pl->cmds.len - 1)
            ctx->last_status = status_from_wait(wstatus);
    }

    free(pids);
    return 0;
}
