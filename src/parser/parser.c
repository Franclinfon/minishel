#include <stdlib.h>
#include <string.h>

#include "minishell/ast/ast.h"
#include "minishell/lexer/lexer.h"
#include "minishell/parser/parser.h"

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

static int node_vec_push(struct ast_node ***items, size_t *len, struct ast_node *n)
{
    struct ast_node **new_items;

    new_items = realloc(*items, sizeof(struct ast_node *) * (*len + 1));
    if (new_items == NULL)
    {
        ast_free(n);
        return -1;
    }

    new_items[*len] = n;
    *items = new_items;
    *len = *len + 1;
    return 0;
}

struct pstate
{
    struct lexer *lx;
    struct token tok;
    int has_tok;
};

static int p_peek(struct pstate *ps, struct token *out)
{
    if (ps->has_tok)
    {
        *out = ps->tok;
        return 0;
    }

    if (lexer_next(ps->lx, &ps->tok) != 0)
    {
        return -1;
    }

    ps->has_tok = 1;
    *out = ps->tok;
    return 0;
}

static int p_consume(struct pstate *ps, struct token *out)
{
    if (p_peek(ps, out) != 0)
    {
        return -1;
    }

    ps->has_tok = 0;
    return 0;
}

static int p_accept(struct pstate *ps, enum token_type t)
{
    struct token tok;

    if (p_peek(ps, &tok) != 0)
    {
        return 0;
    }

    if (tok.type != t)
    {
        return 0;
    }

    (void)p_consume(ps, &tok);
    token_free(&tok);
    return 1;
}

static int parse_command(struct pstate *ps, struct ast_node **out)
{
    struct token tok;
    char **argv;

    *out = NULL;
    argv = NULL;

    while (p_peek(ps, &tok) == 0)
    {
        if (tok.type == TOK_NEWLINE || tok.type == TOK_EOF ||
            tok.type == TOK_SEMI || tok.type == TOK_AND_IF ||
            tok.type == TOK_OR_IF || tok.type == TOK_PIPE)
        {
            break;
        }

        if (tok.type != TOK_WORD)
        {
            argv_free(argv);
            return -1;
        }

        (void)p_consume(ps, &tok);
        if (argv_push(&argv, tok.lexeme) != 0)
        {
            tok.lexeme = NULL;
            token_free(&tok);
            return -1;
        }

        tok.lexeme = NULL;
        token_free(&tok);
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

static void free_items(struct ast_node **items, size_t len)
{
    size_t i;

    i = 0;
    while (i < len)
    {
        ast_free(items[i]);
        i++;
    }
    free(items);
}

static int parse_pipeline(struct pstate *ps, struct ast_node **out)
{
    struct ast_node *cmd;
    struct ast_node **items;
    size_t len;

    items = NULL;
    len = 0;

    if (parse_command(ps, &cmd) != 0 || cmd == NULL)
    {
        ast_free(cmd);
        return -1;
    }

    if (node_vec_push(&items, &len, cmd) != 0)
    {
        free_items(items, len);
        return -1;
    }

    while (p_accept(ps, TOK_PIPE))
    {
        if (parse_command(ps, &cmd) != 0 || cmd == NULL)
        {
            free_items(items, len);
            return -1;
        }

        if (node_vec_push(&items, &len, cmd) != 0)
        {
            free_items(items, len);
            return -1;
        }
    }

    if (len == 1)
    {
        *out = items[0];
        free(items);
        return 0;
    }

    *out = ast_pipeline_new(items, len);
    if (*out == NULL)
    {
        free_items(items, len);
        return -1;
    }

    return 0;
}

static int parse_and_or(struct pstate *ps, struct ast_node **out)
{
    struct ast_node *lhs;
    struct ast_node *rhs;
    struct token tok;

    if (parse_pipeline(ps, &lhs) != 0 || lhs == NULL)
    {
        ast_free(lhs);
        return -1;
    }

    while (p_peek(ps, &tok) == 0 &&
           (tok.type == TOK_AND_IF || tok.type == TOK_OR_IF))
    {
        enum ast_andor_op op;

        op = tok.type == TOK_AND_IF ? AST_AND : AST_OR;
        (void)p_consume(ps, &tok);
        token_free(&tok);

        if (parse_pipeline(ps, &rhs) != 0 || rhs == NULL)
        {
            ast_free(lhs);
            ast_free(rhs);
            return -1;
        }

        lhs = ast_andor_new(op, lhs, rhs);
        if (lhs == NULL)
        {
            return -1;
        }
    }

    *out = lhs;
    return 0;
}

static int parse_list(struct pstate *ps, struct ast_node **out)
{
    struct ast_node *node;
    struct ast_node **items;
    size_t len;

    items = NULL;
    len = 0;

    if (parse_and_or(ps, &node) != 0 || node == NULL)
    {
        ast_free(node);
        return -1;
    }

    if (node_vec_push(&items, &len, node) != 0)
    {
        return -1;
    }

    while (p_accept(ps, TOK_SEMI))
    {
        if (parse_and_or(ps, &node) != 0 || node == NULL)
        {
            size_t i;

            i = 0;
            while (i < len)
            {
                ast_free(items[i]);
                i++;
            }
            free(items);
            return -1;
        }

        if (node_vec_push(&items, &len, node) != 0)
        {
            return -1;
        }
    }

    if (len == 1)
    {
        *out = items[0];
        free(items);
        return 0;
    }

    *out = ast_list_new(items, len);
    if (*out == NULL)
    {
        size_t i;

        i = 0;
        while (i < len)
        {
            ast_free(items[i]);
            i++;
        }
        free(items);
        return -1;
    }

    return 0;
}

int parse_line(struct lexer *lx, struct ast_node **out)
{
    struct pstate ps;
    struct token tok;

    if (lx == NULL || out == NULL)
    {
        return -1;
    }

    ps.lx = lx;
    ps.has_tok = 0;
    *out = NULL;

    if (p_peek(&ps, &tok) != 0)
    {
        return -1;
    }

    if (tok.type == TOK_NEWLINE)
    {
        (void)p_consume(&ps, &tok);
        token_free(&tok);
        *out = NULL;
        return 0;
    }

    if (tok.type == TOK_EOF)
    {
        *out = NULL;
        return 0;
    }

    if (parse_list(&ps, out) != 0)
    {
        return -1;
    }

    if (p_peek(&ps, &tok) != 0)
    {
        ast_free(*out);
        *out = NULL;
        return -1;
    }

    if (tok.type == TOK_NEWLINE)
    {
        (void)p_consume(&ps, &tok);
        token_free(&tok);
    }
    else if (tok.type != TOK_EOF)
    {
        ast_free(*out);
        *out = NULL;
        return -1;
    }

    return 0;
}
