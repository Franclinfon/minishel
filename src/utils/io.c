#include "minishell/io.h"

#include <stdio.h>

int io_open(struct io_ctx *io, int argc, char **argv)
{
    if (io == NULL)
    {
        return -1;
    }

    io->fp = stdin;
    io->should_close = 0;

    if (argc >= 2 && argv != NULL && argv[1] != NULL)
    {
        io->fp = fopen(argv[1], "r");
        if (io->fp == NULL)
        {
            return -1;
        }
        io->should_close = 1;
    }

    return 0;
}

ssize_t io_readline(struct io_ctx *io, char **line, size_t *cap)
{
    if (io == NULL || io->fp == NULL || line == NULL || cap == NULL)
    {
        return -1;
    }

    return getline(line, cap, io->fp);
}

void io_close(struct io_ctx *io)
{
    if (io == NULL)
    {
        return;
    }

    if (io->should_close && io->fp != NULL)
    {
        fclose(io->fp);
    }

    io->fp = NULL;
    io->should_close = 0;
}
