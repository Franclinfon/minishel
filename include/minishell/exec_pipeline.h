#ifndef MINISHELL_EXEC_PIPELINE_H
#define MINISHELL_EXEC_PIPELINE_H

#include "minishell/ast/ast.h"
#include "minishell/sh_ctx.h"

int exec_pipeline(struct sh_ctx *ctx, const struct ast_pipeline *pl);

#endif
