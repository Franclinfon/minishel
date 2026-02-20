#include <stdlib.h>
#include <string.h>

#include "minishell/ast/ast.h"

static void free_argv(char **argv)
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

static void redirs_free(struct ast_redir_list *r)
{
    size_t i;

    if (r == NULL || r->items == NULL)
    {
        return;
    }

    i = 0;
    while (i < r->len)
    {
        free(r->items[i].target);
        i++;
    }

    free(r->items);
    r->items = NULL;
    r->len = 0;
}

static void ast_vec_free(struct ast_vec *v)
{
    size_t i;

    if (v == NULL || v->items == NULL)
    {
        return;
    }

    i = 0;
    while (i < v->len)
    {
        ast_free(v->items[i]);
        i++;
    }

    free(v->items);
    v->items = NULL;
    v->len = 0;
}

struct ast_node *ast_command_new(char **argv, struct ast_redir_list redirs)
{
    struct ast_node *node;

    node = malloc(sizeof(*node));
    if (node == NULL)
    {
        free_argv(argv);
        redirs_free(&redirs);
        return NULL;
    }

    node->kind = AST_COMMAND;
    node->data.command.argv = argv;
    node->data.command.redirs = redirs;
    return node;
}

struct ast_node *ast_pipeline_new(struct ast_node **items, size_t len)
{
    struct ast_node *node;

    node = malloc(sizeof(*node));
    if (node == NULL)
    {
        free(items);
        return NULL;
    }

    node->kind = AST_PIPELINE;
    node->data.pipeline.cmds.items = items;
    node->data.pipeline.cmds.len = len;
    return node;
}

struct ast_node *ast_andor_new(enum ast_andor_op op,
                               struct ast_node *lhs,
                               struct ast_node *rhs)
{
    struct ast_node *node;

    node = malloc(sizeof(*node));
    if (node == NULL)
    {
        ast_free(lhs);
        ast_free(rhs);
        return NULL;
    }

    node->kind = AST_ANDOR;
    node->data.andor.op = op;
    node->data.andor.lhs = lhs;
    node->data.andor.rhs = rhs;
    return node;
}

struct ast_node *ast_list_new(struct ast_node **items, size_t len)
{
    struct ast_node *node;

    node = malloc(sizeof(*node));
    if (node == NULL)
    {
        free(items);
        return NULL;
    }

    node->kind = AST_LIST;
    node->data.list.items.items = items;
    node->data.list.items.len = len;
    return node;
}

void ast_free(struct ast_node *node)
{
    if (node == NULL)
    {
        return;
    }

    if (node->kind == AST_COMMAND)
    {
        free_argv(node->data.command.argv);
        redirs_free(&node->data.command.redirs);
    }
    else if (node->kind == AST_PIPELINE)
    {
        ast_vec_free(&node->data.pipeline.cmds);
    }
    else if (node->kind == AST_ANDOR)
    {
        ast_free(node->data.andor.lhs);
        ast_free(node->data.andor.rhs);
    }
    else if (node->kind == AST_LIST)
    {
        ast_vec_free(&node->data.list.items);
    }

    free(node);
}

static void dump_indent(FILE *out, int depth)
{
    int i;

    i = 0;
    while (i < depth)
    {
        fprintf(out, "  ");
        i++;
    }
}

static void dump_argv(FILE *out, char *const *argv)
{
    size_t i;

    fprintf(out, "[");
    if (argv != NULL)
    {
        i = 0;
        while (argv[i] != NULL)
        {
            if (i != 0)
            {
                fprintf(out, ", ");
            }
            fprintf(out, "\"%s\"", argv[i]);
            i++;
        }
    }
    fprintf(out, "]");
}

static void ast_dump_rec(FILE *out, const struct ast_node *node, int depth)
{
    size_t i;

    if (node == NULL)
    {
        dump_indent(out, depth);
        fprintf(out, "(null)\n");
        return;
    }

    if (node->kind == AST_COMMAND)
    {
        dump_indent(out, depth);
        fprintf(out, "AST_COMMAND argv=");
        dump_argv(out, node->data.command.argv);
        fprintf(out, "\n");
        return;
    }

    if (node->kind == AST_PIPELINE)
    {
        dump_indent(out, depth);
        fprintf(out, "AST_PIPELINE len=%zu\n", node->data.pipeline.cmds.len);
        i = 0;
        while (i < node->data.pipeline.cmds.len)
        {
            ast_dump_rec(out, node->data.pipeline.cmds.items[i], depth + 1);
            i++;
        }
        return;
    }

    if (node->kind == AST_ANDOR)
    {
        dump_indent(out, depth);
        fprintf(out, "AST_ANDOR op=%s\n",
                node->data.andor.op == AST_AND ? "&&" : "||");
        ast_dump_rec(out, node->data.andor.lhs, depth + 1);
        ast_dump_rec(out, node->data.andor.rhs, depth + 1);
        return;
    }

    if (node->kind == AST_LIST)
    {
        dump_indent(out, depth);
        fprintf(out, "AST_LIST len=%zu\n", node->data.list.items.len);
        i = 0;
        while (i < node->data.list.items.len)
        {
            ast_dump_rec(out, node->data.list.items.items[i], depth + 1);
            i++;
        }
        return;
    }

    dump_indent(out, depth);
    fprintf(out, "AST_UNKNOWN\n");
}

void ast_dump(FILE *out, const struct ast_node *node)
{
    if (out == NULL)
    {
        return;
    }

    ast_dump_rec(out, node, 0);
}
