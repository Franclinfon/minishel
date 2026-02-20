#ifndef MINISHELL_AST_H
#define MINISHELL_AST_H

#include <stddef.h>
#include <stdio.h>

enum ast_kind
{
    AST_COMMAND,
    AST_PIPELINE,
    AST_ANDOR,
    AST_LIST
};

enum ast_andor_op
{
    AST_AND,
    AST_OR
};

struct ast_command
{
    char **argv;
};

struct ast_vec
{
    struct ast_node **items;
    size_t len;
};

struct ast_pipeline
{
    struct ast_vec cmds;
};

struct ast_andor
{
    enum ast_andor_op op;
    struct ast_node *lhs;
    struct ast_node *rhs;
};

struct ast_list
{
    struct ast_vec items;
};

struct ast_node
{
    enum ast_kind kind;
    union
    {
        struct ast_command command;
        struct ast_pipeline pipeline;
        struct ast_andor andor;
        struct ast_list list;
    } data;
};

struct ast_node *ast_command_new(char **argv);
struct ast_node *ast_pipeline_new(struct ast_node **items, size_t len);
struct ast_node *ast_andor_new(enum ast_andor_op op,
                               struct ast_node *lhs,
                               struct ast_node *rhs);
struct ast_node *ast_list_new(struct ast_node **items, size_t len);

void ast_free(struct ast_node *node);
void ast_dump(FILE *out, const struct ast_node *node);

#endif /* MINISHELL_AST_H */
