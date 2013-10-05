#include <mpi.h>

#include "fields.h"
#include "simulation.h"

#ifndef PARALLEL_H_INCLUDED
#define PARALLEL_H_INCLUDED

extern MPI_Datatype		MPI_Struct_Field;
extern MPI_Datatype		MPI_Struct_StepResult;
extern MPI_Op			MPI_Op_Sum_StepResult;

enum MessageTag {
	FIELD
};

extern int snd;
extern int rcv;

int init_parallel(int argc, char *argv[]);
void finish_parallel();

int get_rank();
int get_num_processes();

int get_field_process(struct Field *field);
int get_dest_rank(enum Direction direction, int origin);

void send_field(struct Field *field);
void send_field_into_direction(struct Field *field, enum Direction direction);
void send_field_if_border(struct Field *field);

void recv_field();
int irecv_field();
void probe_recv_field();

void exchange_border_fields();

#endif
