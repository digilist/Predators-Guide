CC = /usr/bin/gcc
MPICC = /usr/bin/mpicc
CFLAGS = -Wall -lpthread -lm -std=c99 -g -o predators.out *.c

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
	mpiexec -n $(p) valgrind --tool=memcheck ./predators.out

plot: predators.out
	gnuplot pred.plot
	sleep 1
	evince plot.pdf

.PHONY: clean
clean:
	rm -f *.out
