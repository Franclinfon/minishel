#ifndef MINISHELL_AST_H
#define MINISHELL_AST_H

#include <stdio.h>

enum ast_kind
{
    AST_COMMAND
};

struct ast_command
{
    char **argv;
};

struct ast_node
{
    enum ast_kind kind;
    union
    {
        struct ast_command command;
    } data;
};

struct ast_node *ast_command_new(char **argv);
void ast_free(struct ast_node *node);
void ast_dump(FILE *out, const struct ast_node *node);

#endif /* MINISHELL_AST_H */
