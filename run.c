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
	}

	MPI_Barrier(MPI_COMM_WORLD);

	int i = 0;
	int died = 0;
	while(i < MAX_SIMULATION_STEPS && !died)
	{
		i++;

		if(rank > 0 || num_processes == 1)
		{
			output("Simulation Step %d\n", i);

			simulation_step(map, i);

			struct StepResult *step_result = calculate_step_result(map, i);
			output(" - %d/%d\n", step_result->amount_predators, step_result->amount_prey);

			if(step_result->amount_predators == 0 || step_result->amount_prey == 0)
			{
				output("\nOne species died!\n");
//				died = 1;
			}

			free(step_result);
		}

		MPI_Barrier(MPI_COMM_WORLD);
	}

	output("%d: %d sent\n", get_rank(), sent);
	output("%d: %d rcv\n", get_rank(), rcv);
}

