CC = /usr/bin/gcc
MPICC = /usr/bin/mpicc
MPIEXEC = /usr/bin/mpiexec
CFLAGS = -Wall -lpthread -lm -std=c99 -g -o predators.out *.c

export LD_LIBRARY_PATH=/usr/lib/openmpi/:$LD_LIBRARY_PATH
p=1

all: compile mpi

compile: main.c
	$(MPICC) $(CFLAGS)

run: predators.out
	./predators.out

mpi: predators.out
	$(MPIEXEC) -n $(p) ./predators.out

sync: main.c
	rsync -avze 'ssh' . wr:predators-guide/

valgrind: predators.out
	make compile
	$(MPIEXEC) -n $(p) valgrind --tool=memcheck ./predators.out --leak-check=full --track-origins=yes

plot: predators.out
	gnuplot pred.plot
	sleep 1
	evince plot.pdf

.PHONY: clean
clean:
	rm -f *.out