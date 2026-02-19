#ifndef MINISHELL_VEC_H
#define MINISHELL_VEC_H

#include <stddef.h>

/* Dynamic array of bytes */
struct vec
{
    unsigned char *data;
    size_t len;
    size_t cap;
};

int vec_init(struct vec *v);
void vec_destroy(struct vec *v);

#endif
