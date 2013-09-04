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

	if(parallel)
	{
		// TODO parallel running
	}
	else
	{
		struct SimulationResult *result = runSimulation();
		saveResult(result);
	}

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
