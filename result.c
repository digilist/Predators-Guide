#include <stdio.h>

#include "runtime_configuration.h"
#include "result.h"
#include "config.h"

void saveResult(struct RuntimeConfiguration *config, struct SimulationResult *result)
{
	char filename[255];
	sprintf(filename, "%s/%dx%d-%.0f-%.0f-%.0f.csv", SAVE_PATH, config->mapWidth, config->mapHeight, config->mapFillRate * 100, config->predatorRate * 100, config->preyRate * 100);

	FILE *file = fopen(filename, "a+");

	struct StepResult *r = result->firstStepResult;
	while(r != 0)
	{
		fprintf(file, "%d\t%d\t%d\n", r->current_step, r->amount_predators, r->amount_prey);
		r = r->next;
	}

	fclose(file);
}
