#include <stdlib.h>
#include <string.h>

#include "minishell/lexer/lexer.h"

static int is_ws(char c)
{
    return c == ' ' || c == '\t';
}

static char *dup_range(const char *s, size_t start, size_t end)
{
    size_t len;
    char *out;

    len = end - start;
    out = malloc(len + 1);
    if (out == NULL)
    {
        return NULL;
    }

    memcpy(out, s + start, len);
    out[len] = '\0';
    return out;
}

void lexer_init(struct lexer *lx, const char *input)
{
    lx->input = input;
    lx->pos = 0;
}

static int match(struct lexer *lx, const char *s)
{
    size_t i;

    i = 0;
    while (s[i] != '\0')
    {
        if (lx->input[lx->pos + i] != s[i])
        {
            return 0;
        }
        i++;
    }

    lx->pos += i;
    return 1;
}

int lexer_next(struct lexer *lx, struct token *tok)
{
    const char *in;

    in = lx->input;

    while (is_ws(in[lx->pos]))
    {
        lx->pos++;
    }

    if (in[lx->pos] == '\0')
    {
        tok->type = TOK_EOF;
        tok->lexeme = NULL;
        return 0;
    }

    if (in[lx->pos] == '\n')
    {
        tok->type = TOK_NEWLINE;
        tok->lexeme = NULL;
        lx->pos++;
        return 0;
    }

    if (match(lx, "&&"))
    {
        tok->type = TOK_AND_IF;
        tok->lexeme = NULL;
        return 0;
    }

    if (match(lx, "||"))
    {
        tok->type = TOK_OR_IF;
        tok->lexeme = NULL;
        return 0;
    }

    if (match(lx, ">>"))
    {
        tok->type = TOK_DGREAT;
        tok->lexeme = NULL;
        return 0;
    }

    if (in[lx->pos] == '|')
    {
        tok->type = TOK_PIPE;
        tok->lexeme = NULL;
        lx->pos++;
        return 0;
    }

    if (in[lx->pos] == ';')
    {
        tok->type = TOK_SEMI;
        tok->lexeme = NULL;
        lx->pos++;
        return 0;
    }

    if (in[lx->pos] == '>')
    {
        tok->type = TOK_GREAT;
        tok->lexeme = NULL;
        lx->pos++;
        return 0;
    }

    if (in[lx->pos] == '<')
    {
        tok->type = TOK_LESS;
        tok->lexeme = NULL;
        lx->pos++;
        return 0;
    }    

    /* IONUMBER */
    if (in[lx->pos] >= '0' && in[lx->pos] <= '9')
    {
        size_t start;

        start = lx->pos;
        while (in[lx->pos] >= '0' && in[lx->pos] <= '9')
        {
            lx->pos++;
        }

        if (in[lx->pos] == '>' || in[lx->pos] == '<')
        {
            tok->type = TOK_IONUMBER;
            tok->lexeme = dup_range(in, start, lx->pos);
            return 0;
        }

        lx->pos = start;
    }

    /* WORD */
    {
        size_t start;

        start = lx->pos;
        while (in[lx->pos] != '\0' &&
               !is_ws(in[lx->pos]) &&
               strchr("|;&<>", in[lx->pos]) == NULL)
        {
            lx->pos++;
        }

        tok->type = TOK_WORD;
        tok->lexeme = dup_range(in, start, lx->pos);
        return 0;
    }
}
