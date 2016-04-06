INC = inc/
LIBSRC = src/lib/
APPSRC = src/app/
LIBOBJ = obj/lib/
APPOBJ = obj/app/
BIN = bin/
LIB = libproject.a

LIBOBJS = $(patsubst $(LIBSRC)%.c, $(LIBOBJ)%.o, $(wildcard $(LIBSRC)*.c))
APPOBJS = $(patsubst $(APPSRC)%.c, $(APPOBJ)%.o, $(wildcard $(APPSRC)*.c))
OBJS = $(LIBOBJS) $(APPOBJS)
APPS = $(patsubst $(APPSRC)%.c, $(BIN)%, $(wildcard $(APPSRC)*.c))

CC = gcc

CPPFLAGS = -std=c11 -g -I$(INC)
#CXXFLAGS = -Wall -Wextra -pedantic-errors -Wold-style-cast
LDFLAGS = -L.
LDLIBS = -lproject -lpthread

.PHONY : all clean
.PRECIOUS : $(APPOBJ)%.o

all : folders $(APPS)

folders :
	mkdir -p bin inc src/app src/lib obj/app obj/lib

$(BIN)% : $(APPOBJ)%.o $(LIB)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(LIB) : $(LIBOBJS)
	ar cr $(LIB) $^

$(APPOBJ)%.o : $(APPSRC)%.c
	$(CC) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

$(LIBOBJ)%.o : $(LIBSRC)%.c
	$(CC) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

clean :
	rm -f $(BIN)* $(LIBOBJ)* $(APPOBJ)* $(LIB)
