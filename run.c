#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "map.h"
#include "simulation.h"
#include "parallel.h"

void run_simulation()
{
	struct Map *map = init_map();
	struct Segment *segment = get_simulation_segment(map);

	init_population(map, segment);

	int i = 0;
	while(i < MAX_SIMULATION_STEPS)
	{
		i++;
		printf("Simulation Step %d\n", i);

		simulation_step(map, segment, i);

		struct StepResult *step_result = calculate_step_result(map, segment, i);
		printf(" - %d/%d\n", step_result->amount_predators, step_result->amount_prey);

		if(step_result->amount_predators == 0 || step_result->amount_prey == 0)
		{
			printf("\nOne species died!\n");
			break;
		}

		free(step_result);

	}
}

