#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "config.h"
#include "map.h"
#include "simulation.h"

void initFilesystem();
void createVideo();

/*
 * Applikation zur Simmulation einer Räuber-Beute-Beziehung
 * in einem abgeschlossenen System.
 *
 */
int main()
{
	srand((unsigned) time(0));

	printf("=======================\n");
	printf("|  PREDATOR vs. PREY  |\n");
	printf("=======================\n");

	initFilesystem();

	char buffer[256];
	char abstractBitmapFilepath[256];

	sprintf(abstractBitmapFilepath, "%s%s", BITMAP_PATH, BITMAP_FILENAME);

	struct Map *map = initMap(MAP_WIDTH, MAP_HEIGHT); // for scaling must be square

	sprintf(buffer, abstractBitmapFilepath, 0);
	printToBitmap(map, buffer);

	int i = 1;
	while (1)
	{
		printf("Simulation Step %d\n", i);

		simulationStep(map, i);

		sprintf(buffer, abstractBitmapFilepath, i);
		printToBitmap(map, buffer);

		i++;
		if (i > SIMULATION_STEPS)
			break;
	}

	createVideo();

	return 0;
}

void initFilesystem()
{
	char buffer[256];

	sprintf(buffer, "rm -rf %s*", BITMAP_PATH);
	system(buffer);

	sprintf(buffer, "mkdir -p %s", BITMAP_PATH);
	system(buffer);
}

void createVideo()
{
	char buffer[256];

	sprintf(buffer, "cd %s && ffmpeg -i %%d.bmp -pix_fmt rgb24 output.gif && gifsicle --delay=5 --loop output.gif > pred.gif && rm output.gif", BITMAP_PATH);
	system(buffer);
}
