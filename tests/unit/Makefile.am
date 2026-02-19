AM_CPPFLAGS = -I$(top_srcdir)/include $(CRITERION_CFLAGS)

if HAVE_CRITERION

check_PROGRAMS = unit_tests
TESTS = unit_tests

unit_tests_SOURCES = \
	test_vec.c \
	test_lexer.c \
	test_lexer_more.c

unit_tests_LDADD = \
	$(CRITERION_LIBS) \
	$(top_builddir)/src/utils/libms_utils.a \
	$(top_builddir)/src/lexer/libms_lexer.a

unit_tests_DEPENDENCIES = \
	$(top_builddir)/src/utils/libms_utils.a \
	$(top_builddir)/src/lexer/libms_lexer.a

else
TESTS =
endif
