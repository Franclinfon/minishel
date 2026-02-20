#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "minishell/exec_command.h"

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

static void print_err_open(const char *path)
{
    if (path == NULL)
    {
        return;
    }

    /* Minimal bash-like error line */
    dprintf(2, "minishell: %s: %s\n", path, strerror(errno));
}

static int apply_one_redir(const struct ast_redir *r)
{
    int flags;
    int fd;

    if (r->type == AST_REDIR_IN)
    {
        flags = O_RDONLY;
    }
    else if (r->type == AST_REDIR_OUT)
    {
        flags = O_WRONLY | O_CREAT | O_TRUNC;
    }
    else
    {
        flags = O_WRONLY | O_CREAT | O_APPEND;
    }

    fd = open(r->target, flags, 0666);
    if (fd < 0)
    {
        print_err_open(r->target);
        return -1;
    }

    if (dup2(fd, r->fd) < 0)
    {
        print_err_open(r->target);
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

static int apply_redirs(const struct ast_redir_list *rlist)
{
    size_t i;

    if (rlist == NULL)
    {
        return 0;
    }

    i = 0;
    while (i < rlist->len)
    {
        if (apply_one_redir(&rlist->items[i]) != 0)
        {
            return -1;
        }
        i++;
    }

    return 0;
}

int exec_command(struct sh_ctx *ctx, const struct ast_command *cmd)
{
    pid_t pid;
    int wstatus;

    if (ctx == NULL || cmd == NULL || cmd->argv == NULL || cmd->argv[0] == NULL)
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
        if (apply_redirs(&cmd->redirs) != 0)
        {
            _exit(1);
        }

        execvp(cmd->argv[0], cmd->argv);
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
