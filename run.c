#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <sys/time.h>

#include "helpers.h"
#include "map.h"
#include "simulation.h"
#include "parallel.h"

int get_stats(int step, struct StepResult **last_result, struct SimulationResult *result);
void calc_runtime(struct SimulationResult *result);

struct SimulationResult* run_simulation()
{
	int rank = get_rank();
	int num_processes = get_num_processes();

	struct SimulationResult *result = 0;
	struct StepResult *last_result = 0;

	// only the master process collects the simulation results
	if(rank == 0)
	{
		result = malloc(sizeof(struct SimulationResult));

		// set start time
		gettimeofday(&result->start_time, NULL);
	}

	init_map();

	MPI_Barrier(MPI_COMM_WORLD);

	int i = 0;
	int died = 0;

	if(rank == 0)
	{
		printf("Start Population\n");
	}

	died = get_stats(i, &last_result, result);

	while(i < MAX_SIMULATION_STEPS && !died)
	{
		i++;

		if(rank == 0)
			printf("Simulation Step %d\n", i);

		simulation_step(i);

		died = get_stats(i, &last_result, result);

		if(num_processes == 1)
		{
			print_bitmap(i);
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);

	if(rank == 0)
	{
		gettimeofday(&result->finish_time, NULL);
		calc_runtime(result);

		result->operations = i;
	}


	return result;
}

/**
 * get stats, about this rounds
 *
 * returns 1, if one species died
 */
int get_stats(int step, struct StepResult **last_result, struct SimulationResult *result)
{
	int num_processes = get_num_processes();
	int died = 0;

	struct StepResult step_result = {0, 0, 0, 0}; // global

	// receive stats about this round
	if(num_processes > 1)
	{
		struct StepResult local = {0, 0, 0, 0};

		struct StepResult *tmp = calculate_step_result(step);
		memcpy(&local, tmp, sizeof(struct StepResult));
		free(tmp);

		output("%d step_result %d: %d %d %d\n", get_rank(), local.current_step, local.amount_predators, local.amount_prey, local.amount_plants);

		MPI_Reduce(&local, &step_result, 1, MPI_Struct_StepResult, MPI_Op_Sum_StepResult, 0, MPI_COMM_WORLD);
	}
	else
	{
		struct StepResult *tmp = calculate_step_result(step);
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

		result->operations += step_result.operations;

		printf(" - %d predators / %d prey / %d plants\n", step_result.amount_predators, step_result.amount_prey, step_result.amount_plants);

		died = step_result.amount_predators == 0 || step_result.amount_prey == 0;

		if(died)
		{
			printf("\nOne species died!\n");
		}
	}

	// broadcast to all processes, whether one species died
	MPI_Bcast(&died, 1, MPI_INT, 0, MPI_COMM_WORLD);

	return died;
}

/**
 * calculate the time, the simulation took
 */
void calc_runtime(struct SimulationResult *result)
{
	result->run_time.tv_sec = result->finish_time.tv_sec - result->start_time.tv_sec;
	result->run_time.tv_usec = result->finish_time.tv_usec - result->start_time.tv_usec;

	if(result->run_time.tv_usec < 0)
	{
		result->run_time.tv_sec--;
		result->run_time.tv_usec = result->start_time.tv_usec - result->finish_time.tv_usec;
	}
}
