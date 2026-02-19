#ifndef MINISHELL_LEXER_H
#define MINISHELL_LEXER_H

#include "minishell/lexer/token.h"

struct lexer
{
    const char *input;
    size_t pos;
};

void lexer_init(struct lexer *lx, const char *input);
int lexer_next(struct lexer *lx, struct token *tok);

#endif
