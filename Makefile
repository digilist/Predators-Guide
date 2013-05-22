CC = /usr/bin/gcc
CFLAGS = -Wall -g -o $@.out $<

all: compile run

compile: *.c
	$(CC) $(CFLAGS)

run: predators.out
	./predators.out

.PHONY: clean
clean:
	rm -f *.out
