#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "minishell/builtins.h"
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

static int save_fd(int fd)
{
    int copy;

    copy = dup(fd);
    return copy;
}

static void restore_fd(int fd, int saved)
{
    if (saved >= 0)
    {
        dup2(saved, fd);
        close(saved);
    }
}

static int builtin_in_parent(struct sh_ctx *ctx, const struct ast_command *cmd)
{
    int saved_in;
    int saved_out;
    int saved_err;
    int rc;

    saved_in = save_fd(0);
    saved_out = save_fd(1);
    saved_err = save_fd(2);

    if (saved_in < 0 || saved_out < 0 || saved_err < 0)
    {
        if (saved_in >= 0)
            close(saved_in);
        if (saved_out >= 0)
            close(saved_out);
        if (saved_err >= 0)
            close(saved_err);
        ctx->last_status = 1;
        return -1;
    }

    if (apply_redirs(&cmd->redirs) != 0)
    {
        restore_fd(0, saved_in);
        restore_fd(1, saved_out);
        restore_fd(2, saved_err);
        ctx->last_status = 1;
        return -1;
    }

    rc = builtin_exec(ctx, cmd->argv);

    /* Subject note: fflush(stdout) after builtin. */
    fsync(1);

    restore_fd(0, saved_in);
    restore_fd(1, saved_out);
    restore_fd(2, saved_err);

    return rc;
}

int exec_command_mode(struct sh_ctx *ctx,
                      const struct ast_command *cmd,
                      int allow_parent_builtin)
{
    pid_t pid;
    int wstatus;

    if (ctx == NULL || cmd == NULL || cmd->argv == NULL || cmd->argv[0] == NULL)
    {
        return 0;
    }

    if (allow_parent_builtin && builtin_is(cmd->argv[0]))
    {
        return builtin_in_parent(ctx, cmd);
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

        if (builtin_is(cmd->argv[0]))
        {
            (void)builtin_exec(ctx, cmd->argv);
            _exit(ctx->last_status);
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
