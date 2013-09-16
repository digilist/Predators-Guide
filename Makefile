CC = /usr/bin/gcc
CFLAGS = -Wall -lm -std=c99 -g -o predators.out *.c

all: compile seriell 

compile: main.c
	$(CC) $(CFLAGS)

parallel: predators.out
	./predators.out -p

seriell: predators.out
	./predators.out

valgrind: predators.out
	make compile
	valgrind --leak-check=yes ./predators.out

plot: predators.out
	gnuplot pred.plot

.PHONY: clean
clean:
	rm -f *.out
