#ifndef MINISHELL_SPLIT_WS_H
#define MINISHELL_SPLIT_WS_H

char **split_ws(const char *line);
void split_ws_free(char **argv);

#endif
