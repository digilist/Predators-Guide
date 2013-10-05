#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <pthread.h>

#include "helpers.h"
#include "parallel.h"
#include "fields.h"

#define NUMBER_OF_RCV_THREADS 1

#define _send_field(field, dest_rank) __send_field(field, dest_rank, __func__)

int _rank = -1;
int _num_processes;

pthread_t pthread_rcv[NUMBER_OF_RCV_THREADS];

MPI_Status status;

// extern
MPI_Datatype	MPI_Struct_Field;
MPI_Datatype	MPI_Struct_StepResult;
MPI_Op			MPI_Op_Sum_StepResult;

int snd = 0;
int rcv = 0;

void __send_field(struct Field *field, int dest_rank, const char* caller);

void _create_mpi_types();
void _create_mpi_struct_field();
void _create_mpi_struct_step_result();
void _create_mpi_op_sum_step_results();

int _get_offset(void* t_struct, void* element);

/**
 * init parallel
 *
 */
int init_parallel(int argc, char *argv[])
{
	for(int i = 0; i < NUMBER_OF_RCV_THREADS; i++)
	{
		pthread_rcv[i] = 0;
	}

	int provided;
	MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
	if(provided < MPI_THREAD_SERIALIZED) // .... multiple doesn't work on cluster ?!
	{
		printf("MPI just provied Thread Level %d but needed %d\n", provided, MPI_THREAD_MULTIPLE);
		finish_parallel();
		exit(1);
	}

	_create_mpi_types();

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
	terminate_rcv();

	printf("%d: rcv %d snd %d\n", get_rank(), rcv, snd);
	MPI_Finalize();
}

/**
 * get the process that "owns" the given field
 *
 */
int get_field_process(struct Field *field)
{
	struct Map *map = get_map();

	int col = field->x / (map->width / get_cols());
	int row = field->y / (map->height / get_rows());

	int p = get_rows() * row + col; // plus 1, because the master simulates no process

	return p;
}

/**
 * get the rank of the proccess in direction
 * starting from origin
 */
int get_dest_rank(enum Direction direction, int origin)
{
	// origin will be transformed to destination

	// to simplyfy calculations, will be added later

	int row_start = get_cols() * (origin / get_cols());
	int row_end = get_cols() * (origin / get_cols()) + get_cols() - 1;


	if(direction == LEFT)
	{
		origin--;
		if(origin < row_start)
			origin = row_start + get_cols() - 1;
	}
	else if(direction == RIGHT)
	{
		origin++;
		if(origin > row_end)
			origin = row_start;
	}
	else if(direction == UP)
	{
		origin = origin - get_cols();
		if(origin < 0) // there is no modulo for negative numbers
			origin = (get_rows() * get_cols()) + origin;
	}
	else if(direction == DOWN)
	{
		origin = (origin + get_cols()) % (get_rows() * get_cols());
	}

	if(direction == DOWN_LEFT)
	{
		origin = get_dest_rank(DOWN, get_dest_rank(LEFT, origin));
	}
	else if(direction == DOWN_RIGHT)
	{
		origin = get_dest_rank(DOWN, get_dest_rank(RIGHT, origin));
	}
	else if(direction == UP_LEFT)
	{
		origin = get_dest_rank(UP, get_dest_rank(LEFT, origin));
	}
	else if(direction == UP_RIGHT)
	{
		origin = get_dest_rank(UP, get_dest_rank(RIGHT, origin));
	}

	return origin;
}

void send_field(struct Field *field)
{
	int my_rank = get_rank();

	struct Segment *segment = get_segment();
	struct Map *map = get_map();
	int owner = get_field_process(field);

	if(owner == my_rank)
	{
		send_field_if_border(field);
	}
	else
	{
		// send foreign field to relevant processes
		int left = 0;
		int right = 0;
		int up = 0;
		int down = 0;

		if(field->x < segment->x1)
		{
			if(field->x == 0) // must be the behind right border (floats around the map)
			{
				right = 1;
			}
			else
			{
				left = 1;
			}
		}
		else if(field->x > segment->x2)
		{
			if(field->x == map->width - 1) // must be behind the left border (floats around the map)
			{
				left = 1;
			}
			else
			{
				right = 1;
			}
		}
		else if(field->x == segment->x1)
		{
			left = 1;
		}
		else if(field->x == segment->x2)
		{
			right = 1;
		}

		if(field->y < segment->y1)
		{
			if(field->y == 0) // must be the behind lower border (floats around the map)
			{
				down = 1;
			}
			else
			{
				up = 1;
			}
		}
		else if(field->y > segment->y2)
		{
			if(field->y == map->height - 1) // must be behind the upper border (floats around the map)
			{
				up = 1;
			}
			else
			{
				down = 1;
			}
		}
		else if(field->y == segment->y1)
		{
			up = 1;
		}
		else if(field->y == segment->y2)
		{
			down = 1;
		}

//		printf("%d: send field %dx%d to %d %d %d %d\n", get_rank(), field->x, field->y, left, right, up, down);

		if(left)
			_send_field(field, get_dest_rank(LEFT, my_rank));
		if(right)
			_send_field(field, get_dest_rank(RIGHT, my_rank));
		if(up)
			_send_field(field, get_dest_rank(UP, my_rank));
		if(down)
			_send_field(field, get_dest_rank(DOWN, my_rank));

		if(up && left)
			_send_field(field, get_dest_rank(UP_LEFT, my_rank));
		else if(up && right)
			_send_field(field, get_dest_rank(UP_RIGHT, my_rank));
		else if(down && left)
			_send_field(field, get_dest_rank(DOWN_LEFT, my_rank));
		else if(down && right)
			_send_field(field, get_dest_rank(DOWN_RIGHT, my_rank));
	}
}

/**
 * send a field to another process
 *
 */
void send_field_into_direction(struct Field *field, enum Direction direction)
{
	if(get_rank() == 0)
		return;

	// can be send to multiple processes

	if(direction == UP_LEFT || direction == UP_RIGHT || direction == UP)
	{
		_send_field(field, get_dest_rank(UP, get_rank()));
	}

	if(direction == DOWN_LEFT || direction == DOWN_RIGHT || direction == DOWN)
	{
		_send_field(field, get_dest_rank(DOWN, get_rank()));
	}

	if(direction == UP_LEFT || direction == DOWN_LEFT || direction == LEFT)
	{
		_send_field(field, get_dest_rank(LEFT, get_rank()));
	}

	if(direction == UP_RIGHT || direction == DOWN_RIGHT || direction == RIGHT)
	{
		_send_field(field, get_dest_rank(RIGHT, get_rank()));
	}

	if(direction == UP_RIGHT || direction == UP_LEFT || direction == DOWN_RIGHT || direction == DOWN_LEFT)
	{
		_send_field(field, get_dest_rank(direction, get_rank()));
	}
}

/**
 * helper function to send the field
 */
void __send_field(struct Field *field, int dest_rank, const char* caller)
{
	if(dest_rank != get_rank()) // don't send to myself
	{
		output("%d: send %dx%d (Population Type %d, Age %d) to %d\n", get_rank(), field->x, field->y, field->population_type, field->age, dest_rank);
		MPI_Send(field, 1, MPI_Struct_Field, dest_rank, FIELD, MPI_COMM_WORLD);
		snd++;
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
		output("%d: send border %dx%d into direction %d\n", get_rank(), field->x, field->y, border);
		send_field_into_direction(field, border);
	}
}

/**
 * receive a field from another process
 *
 */
void recv_field()
{
	struct Field rcv_field;
	MPI_Recv(&rcv_field, 1, MPI_Struct_Field, MPI_ANY_SOURCE, FIELD, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	output("%d: received %dx%d, border: %d\n", get_rank(), rcv_field.x, rcv_field.y, is_border_field(&rcv_field));

	rcv++;

	copy_field_to(&rcv_field, get_field(rcv_field.x, rcv_field.y));
}

/**
 * probe if there is a message and receive if
 */
void probe_recv_field()
{
	int fails = 0;
	while(fails < 2)
	{
		int flag = 0;
		MPI_Iprobe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &flag, &status);
		if(flag)
		{
			recv_field();
			fails = 0;
		}
		else
		{
			fails++;
		}
	}
}

/**
 * start a new thread, to receive fields
 */
void start_rcv()
{
	void* rcv_field_thread() {
		while(1)
		{
			recv_field();
		}
		return 0;
	}

	for(int i = 0; i < NUMBER_OF_RCV_THREADS; i++)
	{
		int rc = pthread_create(&pthread_rcv[i], NULL, &rcv_field_thread, 0);
		if (rc){
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}
}

void terminate_rcv()
{
	for(int i = 0; i < NUMBER_OF_RCV_THREADS; i++)
	{
		if(pthread_rcv[i] != 0)
			pthread_cancel(pthread_rcv[i]);
	}
}

/**
 * send information about our border fields
 * and receive information about the adjacent fields
 */
void exchange_border_fields()
{
	if(get_rank() == 0 || get_num_processes() <= 2)
		return;

	struct Segment *segment = get_segment();

	if(get_rows() > 1)
	{
		for(int x = segment->x1; x <= segment->x2; x++)
		{
			// upper border
			send_field(get_field(x, segment->y1));

			// lower border
			send_field(get_field(x, segment->y2));

//			recv_field();
//			recv_field();
		}
	}

	if(get_cols() > 1)
	{
		for(int y = segment->y1; y <= segment->y2; y++)
		{
			// left border
			send_field(get_field(segment->x1, y));

			// right border
			send_field(get_field(segment->x2, y));

			// receive from other processes
//			recv_field();
//			recv_field();
		}
	}

//	for(int i = 0; i < 16; i++)
//		recv_field();
}

/**
 * create the structs, that will be used for MPI communication
 *
 */
void _create_mpi_types()
{
	_create_mpi_struct_field();
	_create_mpi_struct_step_result();
	_create_mpi_op_sum_step_results();
}

void _create_mpi_struct_field()
{
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

void _create_mpi_struct_step_result()
{
	struct StepResult step_result;
	MPI_Datatype types[3] = {MPI_UNSIGNED, MPI_UNSIGNED, MPI_UNSIGNED}; // ignore current_step and *next
	int blocklen[3] = {1, 1, 1};
	MPI_Aint disp[3] = {
		_get_offset(&step_result, &step_result.amount_predators),
		_get_offset(&step_result, &step_result.amount_prey),
		_get_offset(&step_result, &step_result.amount_plants),
	};

	MPI_Type_create_struct(3, blocklen, disp, types, &MPI_Struct_StepResult);
	MPI_Type_commit(&MPI_Struct_StepResult);
}

void _create_mpi_op_sum_step_results()
{
	void sum(struct StepResult *in, struct StepResult *inout, int *len, MPI_Datatype *datatype) {
		struct StepResult r;
		for(int i = 0; i < *len; i++, in++, inout++)
		{
			r.amount_predators = in->amount_predators + inout->amount_predators;
			r.amount_prey = in->amount_prey + inout->amount_prey;
			r.amount_plants = in->amount_plants + inout->amount_plants;
			*inout = r;
		}
	}

	MPI_Op_create((MPI_User_function *)&sum, 1, &MPI_Op_Sum_StepResult);
}

int _get_offset(void* t_struct, void* element)
{
	return (int) (element - t_struct);
}
