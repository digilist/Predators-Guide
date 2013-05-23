CC = /usr/bin/gcc
CFLAGS = -Wall -g -o predators.out *.c

all: compile run

compile: main.c
	$(CC) $(CFLAGS)

run: predators.out
	./predators.out

.PHONY: clean
clean:
	rm -f *.out
