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

struct ast_node *ast_command_new(char **argv)
{
    struct ast_node *node;

    node = malloc(sizeof(*node));
    if (node == NULL)
    {
        free_argv(argv);
        return NULL;
    }

    node->kind = AST_COMMAND;
    node->data.command.argv = argv;
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
    }

    free(node);
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

void ast_dump(FILE *out, const struct ast_node *node)
{
    if (out == NULL)
    {
        return;
    }

    if (node == NULL)
    {
        fprintf(out, "(null)\n");
        return;
    }

    if (node->kind == AST_COMMAND)
    {
        fprintf(out, "AST_COMMAND argv=");
        dump_argv(out, node->data.command.argv);
        fprintf(out, "\n");
        return;
    }

    fprintf(out, "AST_UNKNOWN\n");
}
