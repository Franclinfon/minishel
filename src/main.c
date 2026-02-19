#include <stdlib.h>

#include "minishell/exec_simple.h"
#include "minishell/io.h"
#include "minishell/sh_ctx.h"
#include "minishell/split_ws.h"

static int run_loop(struct sh_ctx *ctx, struct io_ctx *io)
{
    char *line;
    size_t cap;

    line = NULL;
    cap = 0;
    while (io_readline(io, &line, &cap) >= 0)
    {
        char **argv;

        argv = split_ws(line);
        if (argv != NULL)
        {
            (void)exec_simple(ctx, argv);
            split_ws_free(argv);
        }
    }

    free(line);
    return 0;
}

int main(int argc, char **argv)
{
    struct sh_ctx ctx;
    struct io_ctx io;

    sh_ctx_init(&ctx);
    if (io_open(&io, argc, argv) != 0)
    {
        return 1;
    }

    (void)run_loop(&ctx, &io);
    io_close(&io);
    return ctx.last_status;
}
