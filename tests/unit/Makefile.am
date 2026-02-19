AM_CPPFLAGS = -I$(top_srcdir)/include $(CRITERION_CFLAGS)

if HAVE_CRITERION
check_PROGRAMS = unit_tests
TESTS = unit_tests

unit_tests_SOURCES = test_vec.c
unit_tests_LDADD = \
	$(CRITERION_LIBS) \
	$(top_builddir)/src/utils/libms_utils.a
else
TESTS =
endif
