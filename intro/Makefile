CC = gcc
VPATH = ../lib
BIN_DIR = ${PWD}/bin
CFLAGS = -std=gnu99 -ggdb3
CPPFLAGS += -I ../lib
OBJECTS = wrapsock.c readline.c error.c sockfd_to_family.c writen.c readn.c str_echo.c

client: daytimetcpcli.o $(OBJECTS)
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $(BIN_DIR)/$@

server: daytimetcpsrv.o $(OBJECTS)
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $(BIN_DIR)/$@

#daytimetcpcli.o:
#	$(COMPILE.c) $(OUTPUT_OPTION) $<
