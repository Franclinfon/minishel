SUBDIRS = src tests
ACLOCAL_AMFLAGS = -I m4

.PHONY: format format-check tidy tidy-check leak-check

format:
	find src include tests -name '*.[ch]' -print0 | xargs -0 clang-format -i

format-check:
	find src include tests -name '*.[ch]' -print0 | xargs -0 clang-format --Werror --dry-run

# clang-tidy nécessite compile_commands.json (généré en dev via bear -- make)
tidy:
	clang-tidy -p . $$(find src include tests -name '*.[ch]' 2>/dev/null)

tidy-check:
	clang-tidy -p . $$(find src include tests -name '*.[ch]' 2>/dev/null) --warnings-as-errors='*'

# valgrind sur les tests fonctionnels (on pourra étendre)
leak-check:
	VALGRIND=1 $(MAKE) -C tests func
