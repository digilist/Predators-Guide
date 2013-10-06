#include <stdlib.h>
#include <stdio.h>

#include "result.h"
#include "config.h"

void init_filesystem();

void save_result(struct SimulationResult *result)
{
	init_filesystem();

	char filename[255];
	sprintf(filename, "%s/steps.csv", SAVE_PATH);

	FILE *file = fopen(filename, "w");
	if (file != NULL)
	{
		struct StepResult *r = result->first_step_result;
		while(r != 0)
		{
			fprintf(file, "%d\t%d\t%d\t%d\n", r->current_step, r->amount_predators, r->amount_prey, r->amount_plants);
			r = r->next;
		}

		fclose(file);
	}
	else
	{
		printf("error creatnig file %s\n", filename);
		exit(1);
	}

}

void init_filesystem()
{
	char buffer[256];

	sprintf(buffer, "rm -f %s/steps.csv", SAVE_PATH);
	system(buffer);

	sprintf(buffer, "mkdir -p %s", SAVE_PATH);
	system(buffer);
}
