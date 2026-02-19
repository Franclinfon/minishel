AC_PREREQ([2.69])
AC_INIT([minishell], [1.0], [minishell@epita.local])

AM_INIT_AUTOMAKE([foreign subdir-objects -Wall -Werror])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])

AC_PROG_CC
AM_PROG_AR
AC_PROG_RANLIB

CPPFLAGS="$CPPFLAGS -D_POSIX_C_SOURCE=200809L"

# Compilation option
CFLAGS="$CFLAGS -std=c99 -Wall -Wextra -Werror -pedantic -Wvla"

AC_ARG_VAR([CFLAGS], [C compiler flags])
AC_ARG_VAR([CPPFLAGS], [C preprocessor flags])
AC_ARG_VAR([LDFLAGS], [linker flags])
AC_DEFINE([_POSIX_C_SOURCE], [200809L], [Enable POSIX extensions])

AC_CHECK_HEADERS([
  errno.h
  fcntl.h
  signal.h
  stdbool.h
  stddef.h
  stdint.h
  stdio.h
  stdlib.h
  string.h
  sys/types.h
  sys/wait.h
  unistd.h
])

AC_CHECK_FUNCS([
  dup2
  execvp
  fork
  getcwd
  getenv
  isatty
  kill
  open
  pipe
  read
  waitpid
])

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([CRITERION], [criterion],
                  [have_criterion=yes],
                  [have_criterion=no])
AM_CONDITIONAL([HAVE_CRITERION], [test "x$have_criterion" = "xyes"])

AC_CONFIG_FILES([
  Makefile
  src/Makefile
  src/utils/Makefile
  src/lexer/Makefile
  src/parser/Makefile
  src/ast/Makefile
  src/exec/Makefile
  src/builtins/Makefile
  tests/Makefile
  tests/unit/Makefile
  tests/func/Makefile
])

AC_OUTPUT
