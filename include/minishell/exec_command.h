#ifndef MINISHELL_EXEC_COMMAND_H
#define MINISHELL_EXEC_COMMAND_H

#include "minishell/ast/ast.h"
#include "minishell/sh_ctx.h"

int exec_command(struct sh_ctx *ctx, const struct ast_command *cmd);

#endif /* MINISHELL_EXEC_COMMAND_H */
