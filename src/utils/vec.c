#include "minishell/vec.h"

#include <stdlib.h>

int vec_init(struct vec *v)
{
    if (v == NULL)
    {
        return -1;
    }

    v->data = NULL;
    v->len = 0;
    v->cap = 0;
    return 0;
}

void vec_destroy(struct vec *v)
{
    if (v == NULL)
    {
        return;
    }

    free(v->data);
    v->data = NULL;
    v->len = 0;
    v->cap = 0;
}
