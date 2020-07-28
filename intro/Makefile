CC = gcc
VPATH = ../lib
BIN_DIR = ${PWD}/bin
CFLAGS = -std=gnu99 -ggdb3
CPPFLAGS = -I ../lib

client: daytimetcpcli.o readline.c
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $(BIN_DIR)/$@

server: daytimetcpsrv.o
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $(BIN_DIR)/$@

#daytimetcpcli.o:
#	$(COMPILE.c) $(OUTPUT_OPTION) $<
