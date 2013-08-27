CC = /usr/bin/gcc
CFLAGS = -Wall -std=c99 -g -o predators.out *.c

all: compile run

compile: main.c
	$(CC) $(CFLAGS)

run: predators.out
	./predators.out

valgrind: predators.out
		valgrind --leak-check=yes ./predators.out

.PHONY: clean
clean:
	rm -f *.out
