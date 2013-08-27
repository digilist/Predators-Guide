CC = /usr/bin/gcc
CFLAGS = -Wall -lm -std=c99 -g -o predators.out *.c

all: compile run

compile: main.c
	$(CC) $(CFLAGS)

seriell: seriell.c
	$(CC) $(CFLAGS)

run: predators.out
	./predators.out

.PHONY: clean
clean:
	rm -f *.out
