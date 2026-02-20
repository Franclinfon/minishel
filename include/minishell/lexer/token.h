#ifndef MINISHELL_TOKEN_H
#define MINISHELL_TOKEN_H

enum token_type
{
    TOK_IONUMBER,
    TOK_WORD,
    TOK_PIPE,
    TOK_AND_IF,
    TOK_OR_IF,
    TOK_SEMI,
    TOK_GREAT,
    TOK_DGREAT,
    TOK_LESS,
    TOK_NEWLINE,
    TOK_EOF
};

struct token
{
    enum token_type type;
    char *lexeme;
};

void token_free(struct token *tok);

#endif
