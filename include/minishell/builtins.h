#ifndef MINISHELL_BUILTINS_H
#define MINISHELL_BUILTINS_H

#include "minishell/sh_ctx.h"

int builtin_is(const char *name);
int builtin_exec(struct sh_ctx *ctx, char *const argv[]);

#endif
