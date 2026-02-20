#include "minishell/lexer/token.h"

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

void token_free(struct token *tok)
{
    if (tok == NULL)
        return;

    free(tok->lexeme);
    tok->lexeme = NULL;
}

void token_dump(FILE *f, const struct token *tok)
{
    if (f == NULL || tok == NULL)
        return;

    fprintf(f, "TOKEN(%d", tok->type);
    if (tok->lexeme != NULL)
        fprintf(f, ", \"%s\"", tok->lexeme);
    fprintf(f, ")\n");
}
