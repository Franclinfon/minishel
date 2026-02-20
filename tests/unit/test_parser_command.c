#include <criterion/criterion.h>

#include "minishell/lexer/lexer.h"
#include "minishell/parser/parser.h"

Test(parser, empty_line_returns_null)
{
    struct lexer lx;
    struct ast_node *ast;

    lexer_init(&lx, "   \n");
    ast = (void *)0x1;
    cr_assert_eq(parse_line(&lx, &ast), 0);
    cr_assert_eq(ast, NULL);
}

Test(parser, simple_command)
{
    struct lexer lx;
    struct ast_node *ast;

    lexer_init(&lx, "echo hello\n");
    ast = NULL;
    cr_assert_eq(parse_line(&lx, &ast), 0);
    cr_assert_not_null(ast);
    cr_assert_eq(ast->kind, AST_COMMAND);
    cr_assert_str_eq(ast->data.command.argv[0], "echo");
    cr_assert_str_eq(ast->data.command.argv[1], "hello");
    cr_assert_null(ast->data.command.argv[2]);
    ast_free(ast);
}
