#include <stdlib.h>
#include <string.h>

#include "minishell/split_ws.h"

static int is_ws(char c)
{
    return c == ' ' || c == '\t' || c == '\n';
}

static char *ms_strndup(const char *s, size_t n)
{
    char *out;

    out = malloc(n + 1);
    if (out == NULL)
    {
        return NULL;
    }

    memcpy(out, s, n);
    out[n] = '\0';
    return out;
}

static size_t count_tokens(const char *line)
{
    size_t i;
    size_t count;

    i = 0;
    count = 0;
    while (line[i] != '\0')
    {
        while (line[i] != '\0' && is_ws(line[i]))
        {
            i++;
        }
        if (line[i] == '\0')
        {
            break;
        }
        count++;
        while (line[i] != '\0' && !is_ws(line[i]))
        {
            i++;
        }
    }
    return count;
}

static void free_partial(char **argv, size_t n)
{
    size_t i;

    i = 0;
    while (i < n)
    {
        free(argv[i]);
        i++;
    }
    free(argv);
}

char **split_ws(const char *line)
{
    size_t tok_count;
    char **argv;
    size_t i;
    size_t a;

    if (line == NULL)
    {
        return NULL;
    }

    tok_count = count_tokens(line);
    argv = malloc(sizeof(char *) * (tok_count + 1));
    if (argv == NULL)
    {
        return NULL;
    }

    i = 0;
    a = 0;
    while (line[i] != '\0' && a < tok_count)
    {
        size_t start;
        size_t len;

        while (line[i] != '\0' && is_ws(line[i]))
        {
            i++;
        }
        start = i;
        while (line[i] != '\0' && !is_ws(line[i]))
        {
            i++;
        }
        len = i - start;

        argv[a] = ms_strndup(line + start, len);
        if (argv[a] == NULL)
        {
            free_partial(argv, a);
            return NULL;
        }
        a++;
    }

    argv[a] = NULL;
    return argv;
}

void split_ws_free(char **argv)
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
