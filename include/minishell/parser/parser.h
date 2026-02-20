#ifndef MINISHELL_PARSER_H
#define MINISHELL_PARSER_H

#include "minishell/ast/ast.h"
#include "minishell/lexer/lexer.h"

int parse_line(struct lexer *lx, struct ast_node **out);

#endif /* MINISHELL_PARSER_H */
