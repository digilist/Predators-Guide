#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "parallel.h"
#include "map.h"

struct Segment *_segment = 0;

/**
 * init parallel
 *
 */
int init_parallel(int argc, char *argv[])
{
    MPI_Init (&argc, &argv);
    return get_rank();
}

/**
 * get rank / id of current proccess
 *
 */
int get_rank()
{
	int rank;
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);

	return rank;
}

/**
 * get the number of proccess
 *
 */
int get_num_processes()
{
	int num_proccesses;
	MPI_Comm_size(MPI_COMM_WORLD, &num_proccesses);

	return num_proccesses;
}

/**
 * finish parallel
 */
void finish_parallel()
{
	MPI_Finalize();
}

struct Segment* get_simulation_segment(struct Map *map)
{
	if (_segment != 0)
		return _segment;

	int num_processes = get_num_processes();
	int rank = get_rank();

	// if there's only one process, the segment calculation differs a litte bit
	if(num_processes == 1)
		rank = 1;

	int segment_width = map->width / num_processes;
	int segment_height = map->height;

	_segment = malloc(sizeof(struct Segment));

	_segment->x1 = (rank - 1) * segment_width;
	_segment->x2 = rank * segment_width - 1;
	_segment->y1 = 0;
	_segment->y2 = segment_height - 1;
	_segment->width = segment_width;
	_segment->height = segment_height;

	return _segment;
}

void _create_structs()
{
	/*
	 * int MPI_Type_create_struct(
		  int count,
		  int array_of_blocklengths[],
		  MPI_Aint array_of_displacements[],
		  MPI_Datatype array_of_types[],
		  MPI_Datatype *newtype
		);
	 */

	// struct Map
//	MPI_Type_create_struct();
}
