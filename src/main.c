#include <stdlib.h>

#include "minishell/ast/ast.h"
#include "minishell/exec_simple.h"
#include "minishell/io.h"
#include "minishell/lexer/lexer.h"
#include "minishell/parser/parser.h"
#include "minishell/sh_ctx.h"

static int exec_ast(struct sh_ctx *ctx, struct ast_node *node)
{
    size_t i;

    if (ctx == NULL || node == NULL)
    {
        return 0;
    }

    if (node->kind == AST_COMMAND)
    {
        return exec_simple(ctx, node->data.command.argv);
    }

    if (node->kind == AST_LIST)
    {
        i = 0;
        while (i < node->data.list.items.len)
        {
            (void)exec_ast(ctx, node->data.list.items.items[i]);
            i++;
        }
        return 0;
    }

    if (node->kind == AST_ANDOR)
    {
        (void)exec_ast(ctx, node->data.andor.lhs);

        if (node->data.andor.op == AST_AND && ctx->last_status == 0)
        {
            (void)exec_ast(ctx, node->data.andor.rhs);
        }
        else if (node->data.andor.op == AST_OR && ctx->last_status != 0)
        {
            (void)exec_ast(ctx, node->data.andor.rhs);
        }

        return 0;
    }

    if (node->kind == AST_PIPELINE)
    {
        ctx->last_status = 2;
        return -1;
    }

    ctx->last_status = 2;
    return -1;
}

static int run_line(struct sh_ctx *ctx, const char *line)
{
    struct lexer lx;
    struct ast_node *ast;

    lexer_init(&lx, line);

    ast = NULL;
    if (parse_line(&lx, &ast) != 0)
    {
        ctx->last_status = 2;
        ast_free(ast);
        return -1;
    }

    if (ast != NULL)
    {
        (void)exec_ast(ctx, ast);
    }

    ast_free(ast);
    return 0;
}

static int run_loop(struct sh_ctx *ctx, struct io_ctx *io)
{
    char *line;
    size_t cap;

    line = NULL;
    cap = 0;
    while (io_readline(io, &line, &cap) >= 0)
    {
        (void)run_line(ctx, line);
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
