#include <criterion/criterion.h>

#include "minishell/lexer/lexer.h"

Test(lexer, pipe)
{
    struct lexer lx;
    struct token tok;

    lexer_init(&lx, "a|b");

    lexer_next(&lx, &tok);
    cr_assert_eq(tok.type, TOK_WORD);
    token_free(&tok);

    lexer_next(&lx, &tok);
    cr_assert_eq(tok.type, TOK_PIPE);

    lexer_next(&lx, &tok);
    cr_assert_eq(tok.type, TOK_WORD);
}

Test(lexer, ionumber)
{
    struct lexer lx;
    struct token tok;

    lexer_init(&lx, "2>file");

    lexer_next(&lx, &tok);
    cr_assert_eq(tok.type, TOK_IONUMBER);
    token_free(&tok);

    lexer_next(&lx, &tok);
    cr_assert_eq(tok.type, TOK_GREAT);

    lexer_next(&lx, &tok);
    cr_assert_eq(tok.type, TOK_WORD);
}
