#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "parallel.h"
#include "map.h"

struct Segment *_segment = 0;

struct Field rcv_field;

// extern
MPI_Datatype MPI_Struct_Field;

void _create_mpi_structs();
int _get_offset(void* t_struct, void* element);

/**
 * init parallel
 *
 */
int init_parallel(int argc, char *argv[])
{
    MPI_Init (&argc, &argv);

    _create_mpi_structs();

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

/**
 * init the segment for this process and save it to a local variable
 */
void init_segment(struct Map *map)
{
	int num_simulators = get_num_processes();
	int rank = get_rank();

	if(num_simulators > 1)
		num_simulators--;

	// if there's only one process, the segment calculation differs a litte bit
	if(rank == 0)
	{
		if(num_simulators == 1)
		{
			rank = 1; // just for calculations
		}
		else
		{
			printf("Process 0 isn't allowed to get a simulation segment!\n");
			exit(1);
		}
	}

	int segment_width = map->width / num_simulators;
	int segment_height = map->height;

	_segment = malloc(sizeof(struct Segment));

	_segment->x1 = (rank - 1) * segment_width;
	_segment->x2 = rank * segment_width - 1;
	_segment->y1 = 0;
	_segment->y2 = segment_height - 1;
	_segment->width = segment_width;
	_segment->height = segment_height;
}

/**
 * get the segment, in which the simulation for this process should run
 */
struct Segment* get_segment()
{
	return _segment;
}

/**
 * ensure, the destination rank is inside the simulation domain
 *
 */
int get_dest_rank(enum Direction direction)
{
	int dest = get_rank();
	int num_processes = get_num_processes();

	if(direction == LEFT)
	{
		dest = dest -1;
	}
	else if(direction == RIGHT)
	{
		dest = dest + 1;
	}
	else if(direction == UP)
	{
		// TODO currently sent to the current process
	}
	else if(direction == DOWN)
	{
		// TODO currently sent to the current process
	}
	else
	{
		printf("failure\n");
		exit(1);
	}

	if(dest >= num_processes)
		dest = 1;

	if(dest == 0)
		dest = num_processes - 1;

	return dest;
}

/**
 * send a field to another process
 *
 */
void send_field(struct Field *field, enum Direction direction)
{
	if(get_rank() == 0)
		return;

	if(direction == UP_LEFT || direction == UP_RIGHT || direction == UP)
	{
		MPI_Send(field, 1, MPI_Struct_Field, get_dest_rank(UP), FIELD, MPI_COMM_WORLD);
	}

	if(direction == UP_LEFT || direction == DOWN_LEFT || direction == LEFT)
	{
		MPI_Send(field, 1, MPI_Struct_Field, get_dest_rank(LEFT), FIELD, MPI_COMM_WORLD);
	}

	if(direction == UP_RIGHT || direction == DOWN_RIGHT || direction == RIGHT)
	{
		MPI_Send(field, 1, MPI_Struct_Field, get_dest_rank(RIGHT), FIELD, MPI_COMM_WORLD);
	}

	if(direction == DOWN_LEFT || direction == DOWN_RIGHT || direction == DOWN)
	{
		MPI_Send(field, 1, MPI_Struct_Field, get_dest_rank(DOWN), FIELD, MPI_COMM_WORLD);
	}
}

/**
 * send the given field to the other processes, if it is a border field
 *
 */
void send_field_if_border(struct Field *field)
{
	int border = is_border_field(field);
	if(border > -1)
	{
		send_field(field, border);
	}
}

/**
 * receive a field from another process
 *
 */
void recv_field(struct Map *map)
{
	MPI_Recv(&rcv_field, 1, MPI_Struct_Field, MPI_ANY_SOURCE, FIELD, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	copy_field_to(&rcv_field, get_field(map, rcv_field.x, rcv_field.y));
}

/**
 * create the structs, that will be used for MPI communication
 *
 */
void _create_mpi_structs()
{
	/*
	 * int MPI_Type_create_struct(
		  int count,
		  int array_of_blocklengths[],
		  MPI_Aint array_of_displacements[],
		  MPI_Datatype array_of_types[],
		  MPI_Datatype *newtype
		);
	 *
	 */

	// struct Field
	struct Field field;
	MPI_Datatype types[7] = {MPI_UNSIGNED, MPI_UNSIGNED, MPI_INT, MPI_UNSIGNED, MPI_UNSIGNED, MPI_UNSIGNED, MPI_UNSIGNED};
	int blocklen[7] = {1, 1, 1, 1, 1, 1, 1};
	MPI_Aint disp[7] = {
		_get_offset(&field, &field.x),
		_get_offset(&field, &field.y),
		_get_offset(&field, &field.population_type),
		_get_offset(&field, &field.last_step),
		_get_offset(&field, &field.age),
		_get_offset(&field, &field.energy),
		_get_offset(&field, &field.contains_plant),
	};

	MPI_Type_create_struct(7, blocklen, disp, types, &MPI_Struct_Field);
	MPI_Type_commit(&MPI_Struct_Field);
}

int _get_offset(void* t_struct, void* element)
{
	return (int) (element - t_struct);
}
