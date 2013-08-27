#include <stdio.h>
#include <math.h>

#include "runtime_configuration.h"
#include "map.h"
#include "simulation.h"

void simulation(struct RuntimeConfiguration *config);

int main()
{

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

		simulation(config);

		i++;
		next = next->next;
	}

	printf("%d Konfigurationen\n", i);

}

/**
 * führt eine einzelne Simulation mit der gegebenen Konfiguration aus
 *
 */
void simulation(struct RuntimeConfiguration *config)
{
	struct Map *map = initMap(config);

	int i = 0;
	while (i < 10)
	{
		simulationStep(map, i++);
	}
}
