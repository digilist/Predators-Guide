CC = /usr/bin/gcc
MPICC = /usr/bin/mpicc
CFLAGS = -Wall -lm -std=c99 -g -o predators.out *.c

export LD_LIBRARY_PATH=/usr/lib/openmpi/:$LD_LIBRARY_PATH
p=5

all: compile mpi

compile: main.c
	$(MPICC) $(CFLAGS)

run: compile
	./predators.out

mpi: compile
	mpiexec -n $(p) ./predators.out

valgrind: predators.out
	make compile
	valgrind --leak-check=yes ./predators.out

plot: predators.out
	gnuplot pred.plot

.PHONY: clean
clean:
	rm -f *.out
