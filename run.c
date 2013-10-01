#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#include "map.h"
#include "simulation.h"
#include "parallel.h"

int get_stats(struct Map *map, int step, struct StepResult **last_result, struct SimulationResult *result);

struct SimulationResult* run_simulation()
{
	int rank = get_rank();
	int num_processes = get_num_processes();

	struct Map *map;
	struct SimulationResult *result = malloc(sizeof(struct SimulationResult));
	struct StepResult *last_result = 0;

	if(rank > 0 || num_processes == 1)
	{
		map = init_map();
		init_segment(map);

		init_population(map);
	}

	MPI_Barrier(MPI_COMM_WORLD);

	if(rank > 0)
		start_rcv(map);

	int i = 0;
	int died = 0;

	if(rank == 0)
	{
		printf("Start Population\n");
	}
	died = get_stats(map, i, &last_result, result);

	while(i < MAX_SIMULATION_STEPS && !died)
	{
		i++;

		if(rank == 0)
			printf("Simulation Step %d\n", i);

		if(rank > 0 || num_processes == 1)
			simulation_step(map, i);

		MPI_Barrier(MPI_COMM_WORLD);
		died = get_stats(map, i, &last_result, result);

		// barrier to ensure, simulation continues only when all processes are ready
		// not needed, because of MPI_Bcast in get_stats() ?!?
//		MPI_Barrier(MPI_COMM_WORLD);

		int flag = 0;
		MPI_Status status;
		MPI_Iprobe(MPI_ANY_SOURCE, FIELD, MPI_COMM_WORLD, &flag, &status);
	}

	if(rank > 0)
		terminate_rcv();

	return result;
}

/**
 * get stats, about this rounds
 *
 * returns 1, if one species died
 */
int get_stats(struct Map *map, int step, struct StepResult **last_result, struct SimulationResult *result)
{
	int rank = get_rank();
	int num_processes = get_num_processes();
	int died = 0;

	struct StepResult step_result = {0, 0, 0, 0}; // global

	// receive stats about this round
	if(num_processes > 1)
	{
		struct StepResult local_step_result = {0, 0, 0, 0};

		if(rank > 0)
		{
			struct StepResult *tmp = calculate_step_result(map, step);
			memcpy(&local_step_result, tmp, sizeof(struct StepResult));
			free(tmp);
		}

		MPI_Reduce(&local_step_result, &step_result, 1, MPI_Struct_StepResult, MPI_Op_Sum_StepResult, 0, MPI_COMM_WORLD);
	}
	else
	{
		struct StepResult *tmp = calculate_step_result(map, step);
		memcpy(&step_result, tmp, sizeof(struct StepResult));
	}

	if(get_rank() == 0)
	{
		step_result.current_step = step;
		step_result.next = 0;

		if(*last_result == 0)
		{
			*last_result = malloc(sizeof(struct StepResult));
			result->first_step_result = *last_result;
		}
		else
		{
			(*last_result)->next = malloc(sizeof(struct StepResult));
			*last_result = (*last_result)->next;
		}
		**last_result = step_result;

		printf(" - %d predators / %d prey\n", step_result.amount_predators, step_result.amount_prey);

		died = step_result.amount_predators == 0 || step_result.amount_prey == 0;

		if(died)
		{
			printf("\nOne species died!\n");
		}
	}

	// broad cast died to all processes
	MPI_Bcast(&died, 1, MPI_INT, 0, MPI_COMM_WORLD);

	return died;
}
