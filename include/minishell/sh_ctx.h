#ifndef MINISHELL_SH_CTX_H
#define MINISHELL_SH_CTX_H

/* Shell runtime */
struct sh_ctx
{
    int last_status;
};

void sh_ctx_init(struct sh_ctx *ctx);

#endif
