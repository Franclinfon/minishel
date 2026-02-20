#include "minishell/sh_ctx.h"

#include <stddef.h>

void sh_ctx_init(struct sh_ctx *ctx)
{
    if (ctx == NULL)
    {
        return;
    }

    ctx->last_status = 0;
}
