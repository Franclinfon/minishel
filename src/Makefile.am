AM_CPPFLAGS = -I$(top_srcdir)/include
bin_PROGRAMS = minishell

minishell_SOURCES = main.c

# Link
minishell_LDADD = \
	utils/libms_utils.a \
	lexer/libms_lexer.a \
	parser/libms_parser.a \
	ast/libms_ast.a \
	exec/libms_exec.a \
	builtins/libms_builtins.a

SUBDIRS = utils lexer parser ast exec builtins
