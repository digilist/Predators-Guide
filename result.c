#include <stdio.h>

#include "result.h"
#include "config.h"

void saveResult(struct SimulationResult *result)
{
	char filename[255];
	sprintf(filename, "%s/steps.csv", SAVE_PATH);

	FILE *file = fopen(filename, "a+");

	struct StepResult *r = result->firstStepResult;
	while(r != 0)
	{
		fprintf(file, "%d\t%d\t%d\n", r->current_step, r->amount_predators, r->amount_prey);
		r = r->next;
	}

	fclose(file);
}
