#include <mpi.h>
#include "map.h"
#include "simulation.h"

#ifndef PARALLEL_H_INCLUDED
#define PARALLEL_H_INCLUDED

extern MPI_Datatype		MPI_Struct_Field;
extern MPI_Datatype		MPI_Struct_StepResult;
extern MPI_Op			MPI_Op_Sum_StepResult;

enum MessageTag {
	FIELD
};

int init_parallel(int argc, char *argv[]);
void finish_parallel();

int get_rank();
int get_num_processes();

void init_segment(struct Map *map);
struct Segment* get_segment();
int get_cols();
int get_rows();

int get_dest_rank(enum Direction direction, int origin);

void send_field(struct Field *field, enum Direction direction);
void send_field_if_border(struct Field *field);
void recv_field(struct Map *map);
void probe_recv_field(struct Map *map);

void start_rcv(struct Map *map);
void terminate_rcv();

void exchange_border_fields(struct Map *map);

#endif
