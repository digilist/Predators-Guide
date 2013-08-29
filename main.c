#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "config.h"
#include "map.h"
#include "simulation.h"
#include "seriell.h"
#include "result.h"

void initFilesystem();
void createVideo();

/*
 * Applikation zur Simmulation einer Räuber-Beute-Beziehung
 * in einem abgeschlossenen System.
 *
 */
int main(int argc, char* argv[])
{
	srand((unsigned) time(0));
	initFilesystem();

	printf("=======================\n");
	printf("|  PREDATOR vs. PREY  |\n");
	printf("=======================\n");

	int parallel = 0;
	if(argc == 2 && strcmp(argv[1], "-p") == 0)
		parallel = 1;

	struct RuntimeConfigurationList *next = generateRuntimes();
	int i = 0;
	while(next != 0)
	{
		struct RuntimeConfiguration *config = next->config;
		printf("Karte %dx%d, %0.f%% Füllung, %0.f%% Predators, %0.f%% Prey\n",
				config->mapWidth,
				config->mapHeight,
				config->mapFillRate * 100,
				config->predatorRate * 100,
				config->preyRate * 100
		);

		if(parallel)
		{
			// TODO parallel running
		}
		else
		{
			struct SimulationResult *result = runSimulation(config);
			saveResult(config, result);
		}

		i++;
		next = next->next;
	}

//	initFilesystem();
//
//	char buffer[256];
//	char abstractBitmapFilepath[256];
//
//	sprintf(abstractBitmapFilepath, "%s%s", BITMAP_PATH, BITMAP_FILENAME);
//
//	struct Map *map = initMap(MAP_WIDTH, MAP_HEIGHT); // for scaling must be square
//
//	sprintf(buffer, abstractBitmapFilepath, 0);
//	printToBitmap(map, buffer);
//
//	int i = 1;
//
//	struct StepResult *current;
//	struct StepResult *first;
//	struct StepResult *temp_step_result;
//
//	int predator_trend;
//	int prey_trend;
//	int previous_predator_amount;
//	int previous_prey_amount;
//
//	while (1)
//	{
//		printf("Simulation Step %d\n", i);
//		if (i == 1) {
//			first = simulationStep(map, i);
//			current = first;
//			temp_step_result = first;
//
//			previous_predator_amount = first->amount_predators;
//			previous_prey_amount = first->amount_prey;
//		} else {
//			temp_step_result = simulationStep(map, i);
//
//			if (i > 2) { // there can be no turning point in the first two rounds
//
//				// check on turning point
//				if (((((previous_predator_amount - temp_step_result->amount_predators) < 0) != predator_trend)
//					&& (previous_predator_amount - temp_step_result->amount_predators) != 0)
//					|| ((((previous_prey_amount - temp_step_result->amount_prey) < 0) != prey_trend)
//					&& (previous_prey_amount - temp_step_result->amount_prey) != 0)) {
//
//					// attach result set to chained list
//					current->next = temp_step_result;
//					current = current->next;
//
//					printf("\nTURNING POINT %d/%d\n\n", previous_predator_amount, previous_prey_amount);
//				}
//			}
//
//			predator_trend = (previous_predator_amount - temp_step_result->amount_predators) < 0;
//			prey_trend = (previous_prey_amount - temp_step_result->amount_prey) < 0;
//			previous_predator_amount = temp_step_result->amount_predators;
//			previous_prey_amount = temp_step_result->amount_prey;
//
//		}
//
//
//		if (temp_step_result->amount_predators == 0 || temp_step_result->amount_prey == 0) {
//			printf("One species died!\n");
//			break;
//		}
//		printf("%d/%d - ", temp_step_result->amount_predators, temp_step_result->amount_prey);
//
//
//		sprintf(buffer, abstractBitmapFilepath, i);
//		printToBitmap(map, buffer);
//
//		i++;
//		if (i > MAX_SIMULATION_STEPS)
//			break;
//	}

	//createVideo();

	return 0;
}

void initFilesystem()
{
	char buffer[256];

	sprintf(buffer, "rm -rf %s*", SAVE_PATH);
	system(buffer);

	sprintf(buffer, "mkdir -p %s", SAVE_PATH);
	system(buffer);
}

void createVideo()
{
	char buffer[256];

	sprintf(buffer, "cd %s && ffmpeg -i %%d.bmp -pix_fmt rgb24 output.gif && gifsicle --delay=5 --loop output.gif > pred.gif && rm output.gif", SAVE_PATH);
	system(buffer);
}
