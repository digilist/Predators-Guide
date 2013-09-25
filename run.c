#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#include "map.h"
#include "simulation.h"
#include "parallel.h"

int num_processes;
int rank;

struct SimulationResult* run_simulation()
{
	rank = get_rank();
	num_processes = get_num_processes();

	struct Map *map;
	struct SimulationResult *result = malloc(sizeof(struct SimulationResult));
	struct StepResult *last_result = 0;

	if(rank > 0 || num_processes == 1)
	{
		map = init_map();
		init_segment(map);

		init_population(map);
	}

	printf("%d: start simulation\n", get_rank());

	int i = 0;
	int died = 0;
	while(i < MAX_SIMULATION_STEPS && !died)
	{
		MPI_Barrier(MPI_COMM_WORLD);
		i++;

		if(rank > 0 || num_processes == 1)
		{
			output("Simulation Step %d\n", i);
			simulation_step(map, i);
		}
		else
		{
			// i am the master
			printf("Simulation Step %d\n", i);
		}

		MPI_Barrier(MPI_COMM_WORLD);

		// receive stats about this round
		if(num_processes > 1)
		{
			struct StepResult step_result = {0, 0, 0, 0}; // global
			struct StepResult local_step_result = {0, 0, 0, 0};

			if(rank > 0)
			{
				struct StepResult *tmp = calculate_step_result(map, i);
				memcpy(&local_step_result, tmp, sizeof(struct StepResult));
				free(tmp);
			}

			MPI_Reduce(&local_step_result, &step_result, 1, MPI_Struct_StepResult, MPI_Op_Sum_StepResult, 0, MPI_COMM_WORLD);

			if(rank == 0)
			{
				step_result.current_step = i;
				step_result.next = 0;

				if(last_result == 0)
				{
					last_result = malloc(sizeof(struct StepResult));
					result->first_step_result = last_result;
				}
				else
				{
					last_result->next = malloc(sizeof(struct StepResult));
					last_result = last_result->next;
				}
				*last_result = step_result;

				printf(" - %d/%d\n", step_result.amount_predators, step_result.amount_prey);
				if(step_result.amount_predators == 0 || step_result.amount_prey == 0)
				{
					printf("\nOne species died!\n");
//					died = 1; // TODO send die to all processes
				}
			}
		}
	}

	return result;
}

