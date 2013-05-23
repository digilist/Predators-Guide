#include <stdio.h>
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

	initField(3, 3);

	return 0;
}
