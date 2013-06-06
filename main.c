#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
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

	char abstractBitmapFilepath[256];
	sprintf(abstractBitmapFilepath, "%s%s", BITMAP_PATH, BITMAP_FILENAME);

	char buffer[256];
	char filename[256];

	sprintf(buffer, "rm -rf %s*", BITMAP_PATH);
	system(buffer);

	sprintf(buffer, "mkdir -p %s", BITMAP_PATH);
	system(buffer);

	struct Map *map = initMap(MAP_WIDTH, MAP_HEIGHT); // for scaling must be square

	sprintf(filename, abstractBitmapFilepath, 0);
	printToBitmap(map, filename);

	int i = 1;
	while (1)
	{
		printf("Simulation Step %d\n", i);

		simulationStep(map, i);

		sprintf(filename, abstractBitmapFilepath, i);
		printToBitmap(map, filename);

		i++;
		if (i > SIMULATION_STEPS)
			break;
	}

	sprintf(buffer, "cd %s && ffmpeg -i %%d.bmp -pix_fmt rgb24 output.gif && gifsicle --delay=5 --loop output.gif > pred.gif && rm output.gif", BITMAP_PATH);
	system(buffer);

	// and then
	/*	ffmpeg -i %d.bmp -pix_fmt rgb24 output.gif && \
		gifsicle --delay=5 --loop output.gif > pred.gif
	 */

	return 0;
}
