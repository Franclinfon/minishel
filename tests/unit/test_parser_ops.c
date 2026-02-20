#include <criterion/criterion.h>

#include "minishell/lexer/lexer.h"
#include "minishell/parser/parser.h"

Test(parser, list_semicolon)
{
    struct lexer lx;
    struct ast_node *ast;

    lexer_init(&lx, "a;b\n");
    ast = NULL;
    cr_assert_eq(parse_line(&lx, &ast), 0);
    cr_assert_not_null(ast);
    cr_assert_eq(ast->kind, AST_LIST);
    cr_assert_eq(ast->data.list.items.len, 2);
    ast_free(ast);
}

Test(parser, and_or_and)
{
    struct lexer lx;
    struct ast_node *ast;

    lexer_init(&lx, "a&&b\n");
    ast = NULL;
    cr_assert_eq(parse_line(&lx, &ast), 0);
    cr_assert_not_null(ast);
    cr_assert_eq(ast->kind, AST_ANDOR);
    cr_assert_eq(ast->data.andor.op, AST_AND);
    ast_free(ast);
}

Test(parser, and_or_or)
{
    struct lexer lx;
    struct ast_node *ast;

    lexer_init(&lx, "a||b\n");
    ast = NULL;
    cr_assert_eq(parse_line(&lx, &ast), 0);
    cr_assert_not_null(ast);
    cr_assert_eq(ast->kind, AST_ANDOR);
    cr_assert_eq(ast->data.andor.op, AST_OR);
    ast_free(ast);
}

Test(parser, pipeline_parses)
{
    struct lexer lx;
    struct ast_node *ast;

    lexer_init(&lx, "a|b|c\n");
    ast = NULL;
    cr_assert_eq(parse_line(&lx, &ast), 0);
    cr_assert_not_null(ast);

    /* Could be AST_PIPELINE or collapsed if len==1, but here it's >1. */
    cr_assert_eq(ast->kind, AST_PIPELINE);
    cr_assert_eq(ast->data.pipeline.cmds.len, 3);
    ast_free(ast);
}
