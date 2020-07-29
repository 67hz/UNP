CC = gcc
BIN_DIR = ${PWD}/bin
CFLAGS = -std=gnu99 -ggdb3

test: test.c
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $(BIN_DIR)/$@

