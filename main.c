#include <stdio.h>
#include <stdlib.h>
#include "map.h"
#include "simulation.h"

/*
 * Applikation zur Simmulation einer Räuber-Beute-Beziehung
 * in einem abgeschlossenen System.
 *
 */
int main()
{
	printf("=======================\n");
	printf("|  PREDATOR vs. PREY  |\n");
	printf("=======================\n");

	system("rm -rf /tmp/pred/*");
	system("mkdir -p /tmp/pred");

	struct Map *map = initMap(100, 100); // for scaling must be square
	printToBitmap(map, "/tmp/pred/0.bmp");

	int i = 1;
	while (1)
	{
		printf("Simulation Step %d\n", i);

		simulationStep(map, i);

		char filename[256];
		snprintf(filename, sizeof filename, "/tmp/pred/%d.bmp", i);

		printToBitmap(map, filename);

		i++;
		if (i > 30)
			break;
	}

	return 0;
}
