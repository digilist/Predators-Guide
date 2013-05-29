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

	initField(20, 20);

	createBitmap();

	return 0;
}

void createBitmap() {
	int width = 1000;
	int height = 1000;

	char *bmp;
	bmp = malloc(sizeof(char) * width * height * 3);

	for(int i = 0; i < width * height * 3; i++)
	{
		bmp[i] = rand() % 256;
	}

	write_bmp("/tmp/test.bmp", width, height, bmp);
}
