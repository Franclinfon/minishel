#include <criterion/criterion.h>

#include "minishell/lexer/lexer.h"

Test(lexer, simple_words)
{
    struct lexer lx;
    struct token tok;

    lexer_init(&lx, "echo hello");

    lexer_next(&lx, &tok);
    cr_assert_eq(tok.type, TOK_WORD);
    token_free(&tok);

    lexer_next(&lx, &tok);
    cr_assert_eq(tok.type, TOK_WORD);
    token_free(&tok);

    lexer_next(&lx, &tok);
    cr_assert_eq(tok.type, TOK_EOF);
}
