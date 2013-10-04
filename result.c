#include <stdio.h>

#include "result.h"
#include "config.h"

void save_result(struct SimulationResult *result)
{
	char filename[255];
	sprintf(filename, "%s/steps.csv", SAVE_PATH);

	FILE *file = fopen(filename, "w");

	struct StepResult *r = result->first_step_result;
	while(r != 0)
	{
		fprintf(file, "%d\t%d\t%d\t%d\n", r->current_step, r->amount_predators, r->amount_prey, r->amount_plants);
		r = r->next;
	}

	fclose(file);
}
