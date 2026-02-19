#ifndef MINISHELL_IO_H
#define MINISHELL_IO_H

#include <stdio.h>
#include <stddef.h>
#include <sys/types.h>

struct io_ctx
{
    FILE *fp;
    int should_close;
};

int io_open(struct io_ctx *io, int argc, char **argv);
ssize_t io_readline(struct io_ctx *io, char **line, size_t *cap);
void io_close(struct io_ctx *io);

#endif
