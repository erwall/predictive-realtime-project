INC = inc/
LIBSRC = src/lib/
APPSRC = src/app/
LIBOBJ = obj/lib/
APPOBJ = obj/app/
BIN = bin/
LIB = libproject.a

LIBOBJS = $(patsubst $(LIBSRC)%.c, $(LIBOBJ)%.o, $(wildcard $(LIBSRC)*.c))
LIBOBJS += $(patsubst $(LIBSRC)%.cc, $(LIBOBJ)%.o, $(wildcard $(LIBSRC)*.cc))
APPOBJS = $(patsubst $(APPSRC)%.c, $(APPOBJ)%.o, $(wildcard $(APPSRC)*.c))
APPOBJS += $(patsubst $(APPSRC)%.cc, $(APPOBJ)%.o, $(wildcard $(APPSRC)*.cc))
OBJS = $(LIBOBJS) $(APPOBJS)
APPS = $(patsubst $(APPSRC)%.c, $(BIN)%, $(wildcard $(APPSRC)*.c))
APPS += $(patsubst $(APPSRC)%.cc, $(BIN)%, $(wildcard $(APPSRC)*.cc))

CC = gcc
CPPC = g++

CFLAGS = -std=c99 -g -I$(INC)
CPPFLAGS = -std=c++11 -g -I$(INC)
#CXXFLAGS = -Wall -Wextra -pedantic-errors -Wold-style-cast
LDFLAGS = -L.
LDLIBS = -lproject -lpthread -lGL

.PHONY : all clean
.PRECIOUS : $(APPOBJ)%.o

all : folders $(APPS)

folders :
	mkdir -p bin inc src/app src/lib obj/app obj/lib

$(BIN)% : $(APPOBJ)%.o $(LIB)
	$(CPPC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(LIB) : $(LIBOBJS)
	ar cr $(LIB) $^

$(APPOBJ)%.o : $(APPSRC)%.c
	$(CC) -c $(CFLAGS) $(CXXFLAGS) $< -o $@

$(LIBOBJ)%.o : $(LIBSRC)%.c
	$(CC) -c $(CFLAGS) $(CXXFLAGS) $< -o $@

$(APPOBJ)%.o : $(APPSRC)%.cc
	$(CPPC) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

$(LIBOBJ)%.o : $(LIBSRC)%.cc
	$(CPPC) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

clean :
	rm -f $(BIN)* $(LIBOBJ)* $(APPOBJ)* $(LIB)
