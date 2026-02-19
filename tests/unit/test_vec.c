#include <criterion/criterion.h>

#include "minishell/vec.h"

Test(vec, init_destroy)
{
    struct vec v;

    cr_assert_eq(vec_init(&v), 0);
    vec_destroy(&v);
    cr_assert_eq(v.data, NULL);
}
