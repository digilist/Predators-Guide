#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "parallel.h"
#include "map.h"

struct Segment *_segment = 0;
int _rank = -1;
int _num_processes;

MPI_Status status;

// extern
MPI_Datatype MPI_Struct_Field;

void _send_field(struct Field *field, int dest_rank);
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
	// häufigeres Abfragen hat Anscheinend Seiteneffekte?!
	if(_rank > -1)
		return _rank;

	MPI_Comm_rank(MPI_COMM_WORLD, &_rank);
	return _rank;
}

/**
 * get the number of proccess
 *
 */
int get_num_processes()
{
	if(_num_processes > 0)
		return _num_processes;

	MPI_Comm_size(MPI_COMM_WORLD, &_num_processes);
	return _num_processes;
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
			output("Process 0 isn't allowed to get a simulation segment!\n");
			exit(1);
		}
	}

	int rows = 1;
	int cols = 1;

	if(num_simulators > 1)
	{
		// calculate number of segments, seperated in rows and columns
		// this algorithm is based on prime factorization
		int n = num_simulators;
		int div = 2;
		int primes[10]; // under the assumption, there are max. 10 prime factors in the domain of n / num_simulators
		int p = 0; // highest index in primes

		while(n % div != 0)
			div++;

		while(n % div == 0)
		{
			primes[p] = div;

			if(n == div && (p == 0 || 1)) // primes and end
			{
				break;
			}

			n = n / div;
			p++;

			while(n % div != 0)
				div++;
		}

		if (p == 0) // prime
		{
			primes[1] = 1;
			p++;
		}

		for(int i = 1; i < p; i++)
		{
			primes[0] *= primes[i];
			if(p - 1 > i)
			{
				primes[p - 1] *= primes[p];
				p--;
			}
		}
		primes[1] = primes[p];

		cols = primes[0];
		rows = primes[1];
	}

	if(rank == 1)
	{
		printf("Splitting Map into %d cols and %d rows\n", cols, rows);
	}

	int segment_width = map->width / cols;
	int segment_height = map->height / rows;

	// if cols not devide width make the last column a little bigger
	if(map->width % cols != 0)
	{
		// but only if this process simulates a segment in the last column
		if(rank % cols == 0)
		{
			segment_width += map->width - cols * segment_width;
		}
	}

	// if rows not devide height make the last column a little bigger
	if(map->height % rows != 0)
	{
		// but only if this process simulates a segment in the last row
		if(rank > (rows-1) * cols)
		{
			segment_height += map->height - rows * segment_height;
		}
		// but only if this process simulates a last row in a column
		printf("height\n");
	}

	_segment = malloc(sizeof(struct Segment));

	_segment->x1 = (rank - 1) * segment_width;
	_segment->x2 = rank * segment_width - 1;
	_segment->y1 = 0;
	_segment->y2 = segment_height - 1;
	_segment->width = segment_width;
	_segment->height = segment_height;

	printf("Process %d Segment: %d:%d x %d:%d \n", get_rank(), _segment->x1, _segment->x2, _segment->y1, _segment->y2);
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
		output("failure\n");
		exit(1);
	}

	if(dest >= num_processes)
		dest = 1;

	if(dest == 0)
		dest = num_processes - 1;

	return dest;
}

int sent;

/**
 * send a field to another process
 *
 */
void send_field(struct Field *field, enum Direction direction)
{
	if(get_rank() == 0)
		return;

//	output("%d: send %dx%d : %d\n", get_rank(), field->x, field->y, direction);

	if(direction == UP_LEFT || direction == UP_RIGHT || direction == UP)
	{
		_send_field(field, get_dest_rank(UP));
	}

	if(direction == DOWN_LEFT || direction == DOWN_RIGHT || direction == DOWN)
	{
		_send_field(field, get_dest_rank(DOWN));
	}

	if(direction == UP_LEFT || direction == DOWN_LEFT || direction == LEFT)
	{
		_send_field(field, get_dest_rank(LEFT));
	}

	if(direction == UP_RIGHT || direction == DOWN_RIGHT || direction == RIGHT)
	{
		_send_field(field, get_dest_rank(RIGHT));
	}
}

/**
 * helper function to send the field
 */
void _send_field(struct Field *field, int dest_rank)
{
	if(dest_rank != get_rank())
	{
		MPI_Send(field, 1, MPI_Struct_Field, dest_rank, FIELD, MPI_COMM_WORLD);
		sent++;
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
//		output("%d: send border %dx%d : %d\n", get_rank(), field->x, field->y, border);
		send_field(field, border);
	}
}

int rcv;

/**
 * receive a field from another process
 *
 */
void recv_field(struct Map *map)
{
	struct Field rcv_field;
	MPI_Recv(&rcv_field, 1, MPI_Struct_Field, MPI_ANY_SOURCE, FIELD, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

//	output("%d: received %dx%d, border: %d\n", get_rank(), rcv_field.x, rcv_field.y, is_border_field(&rcv_field));

	rcv++;

	copy_field_to(&rcv_field, get_field(map, rcv_field.x, rcv_field.y));
}

/**
 * probe if there is a message and receive if
 */
void probe_recv_field(struct Map *map)
{
	int fails = 0;
	while(fails < 2)
	{
		int flag = 0;
		MPI_Iprobe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &flag, &status);
		if(flag)
		{
			recv_field(map);
			fails = 0;
		}
		else
		{
			fails++;
		}

	}
}

/**
 * send information about our border fields
 * and receive information about the adjacent fields
 */
void exchange_border_fields(struct Map *map)
{
	if(get_rank() == 0 || get_num_processes() <= 2)
		return;

	struct Segment *segment = get_segment(map);

	// TODO
//	for(int x = segment->x1; x <= segment->x2; x++)
//	{
//
//	}

	for(int y = segment->y1; y <= segment->y2; y++)
	{
		// left border
		send_field(get_field(map, segment->x1, y), LEFT);

		// right border
		send_field(get_field(map, segment->x2, y), RIGHT);
	}

	// and now receive from other processes
	for(int y = segment->y1; y <= segment->y2; y++)
	{
		// receive one field per line, because there are two sides
		recv_field(map);
		recv_field(map);
	}
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
