#include <stdlib.h>
#include <string.h>

#include "minishell/ast/ast.h"
#include "minishell/lexer/lexer.h"
#include "minishell/parser/parser.h"

static size_t argv_count(char *const *argv)
{
    size_t n;

    n = 0;
    if (argv == NULL)
    {
        return 0;
    }

    while (argv[n] != NULL)
    {
        n++;
    }

    return n;
}

static void argv_free(char **argv)
{
    size_t i;

    if (argv == NULL)
    {
        return;
    }

    i = 0;
    while (argv[i] != NULL)
    {
        free(argv[i]);
        i++;
    }
    free(argv);
}

static int argv_push(char ***argvp, char *word)
{
    char **argv;
    size_t n;
    char **new_argv;

    argv = *argvp;
    n = argv_count(argv);

    new_argv = realloc(argv, sizeof(char *) * (n + 2));
    if (new_argv == NULL)
    {
        free(word);
        argv_free(argv);
        *argvp = NULL;
        return -1;
    }

    new_argv[n] = word;
    new_argv[n + 1] = NULL;
    *argvp = new_argv;
    return 0;
}

static int parse_command_words(struct lexer *lx, char ***argvp)
{
    struct token tok;

    *argvp = NULL;

    while (lexer_next(lx, &tok) == 0)
    {
        if (tok.type == TOK_NEWLINE || tok.type == TOK_EOF)
        {
            token_free(&tok);
            return 0;
        }

        if (tok.type != TOK_WORD)
        {
            token_free(&tok);
            argv_free(*argvp);
            *argvp = NULL;
            return -1;
        }

        if (argv_push(argvp, tok.lexeme) != 0)
        {
            tok.lexeme = NULL;
            token_free(&tok);
            return -1;
        }

        tok.lexeme = NULL;
        token_free(&tok);
    }

    argv_free(*argvp);
    *argvp = NULL;
    return -1;
}

int parse_line(struct lexer *lx, struct ast_node **out)
{
    char **argv;

    if (lx == NULL || out == NULL)
    {
        return -1;
    }

    *out = NULL;

    if (parse_command_words(lx, &argv) != 0)
    {
        return -1;
    }

    if (argv == NULL || argv[0] == NULL)
    {
        argv_free(argv);
        *out = NULL;
        return 0;
    }

    *out = ast_command_new(argv);
    if (*out == NULL)
    {
        return -1;
    }

    return 0;
}
