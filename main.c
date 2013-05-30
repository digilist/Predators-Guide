#include <stdio.h>
#include <stdlib.h>
#include "map.h"
#include "simulation.h"

/*
 * Applikation zur Simmulation einer Räuber-Beute-Beziehung
 * in einem abgeschlossenen System.
 *
 */
int main() {
	printf("=======================\n");
	printf("|  PREDATOR vs. PREY  |\n");
	printf("=======================\n");

	struct Map *map = initMap(100, 100);
	printToBitmap(map);

	int i = 0;
	while(1)
	{
		simulationStep(map);
		//printToBitmap(map);
		printf("%d\n", i++);
	}

	return 0;
}
