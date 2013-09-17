#include "map.h"

#ifndef PARALLEL_H_INCLUDED
#define PARALLEL_H_INCLUDED

int init_parallel(int argc, char *argv[]);
void finish_parallel();

int get_rank();
int get_num_processes();

struct Segment* get_simulation_segment(struct Map *map);

#endif
