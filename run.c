#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#include "map.h"
#include "simulation.h"
#include "parallel.h"

void exchange_border_fields(struct Map *map);

int num_processes;
int rank;

void run_simulation()
{
	rank = get_rank();
	num_processes = get_num_processes();

	struct Map *map;

	if(rank > 0 || num_processes == 1)
	{
		map = init_map();
		init_segment(map);

		init_population(map);
		exchange_border_fields(map);
	}

	MPI_Barrier(MPI_COMM_WORLD);

	int i = 0;
	while(i < MAX_SIMULATION_STEPS)
	{
		i++;

		if(rank > 0 || num_processes == 1)
		{
			printf("Simulation Step %d\n", i);

			simulation_step(map, i);

//			struct StepResult *step_result = calculate_step_result(map, i);
//			printf(" - %d/%d\n", step_result->amount_predators, step_result->amount_prey);
//
//			if(step_result->amount_predators == 0 || step_result->amount_prey == 0)
//			{
//				printf("\nOne species died!\n");
//				break;
//			}
//
//			free(step_result);
		}

		MPI_Barrier(MPI_COMM_WORLD);
	}
}

/**
 * send information about our border fields
 * and receive information about the adjacent fields
 */
void exchange_border_fields(struct Map *map)
{
	if(rank == 0)
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
		send_field(get_field(map, segment->x1, y), RIGHT);
	}

	// and now receive from other processes

	for(int y = segment->y1; y <= segment->y2; y++)
	{
		// receive one field per line, because there are two sides

		recv_field(map);
		recv_field(map);
	}
}

