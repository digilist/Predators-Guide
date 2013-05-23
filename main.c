#include <stdio.h>
#include "field.h"


/*
 * Applikation zur Simmulation einer Räuber-Beute-Beziehung
 * in einem abgeschlossenen System.
 *
 */
int main() {
	printf("=======================\n");
	printf("|  PREDATOR vs. PRAY  |\n");
	printf("=======================\n");

	initField(100, 100);

	return 0;
}
