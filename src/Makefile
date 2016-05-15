all: io

io: io.c
	gcc -g -o io_test -fPIC \
	-Wall -Werror \
	io.c \
	-lcomedi -lm

clean:
	rm -f io_test
