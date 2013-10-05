#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>

#include "config.h"
#include "map.h"
#include "run.h"
#include "result.h"

#include "parallel.h"

void create_video();

/*
 * Applikation zur Simmulation einer Räuber-Beute-Beziehung
 * in einem abgeschlossenen System.
 *
 */
int main(int argc, char *argv[])
{
	int rank = init_parallel(argc, argv);

	// init random and inluce the process rank (rank+1, to prevent a 0-result for the root process)
	srand((unsigned) time(0) * (rank + 1));
//	srand((unsigned) rank);

	if(rank == 0)
	{
		printf("+------------------------------------------+\n");
		printf("| PREDATOR vs. PREY                        |\n");
		printf("+------------------------------------------+\n");
		printf("| Running simulation with %02d processes     |\n", get_num_processes());
		printf("+------------------------------------------+\n");
	}

	struct SimulationResult *result = run_simulation();
	finish_parallel();

	if(rank == 0 && result != 0)
	{
		printf("Simulation finished in %d:%.9ds.\n", (int) result->run_time.tv_sec, (int) result->run_time.tv_usec);
		save_result(result);
	}

	return 0;
}

void create_video()
{
	char buffer[256];

	sprintf(buffer, "cd %s && ffmpeg -i %%d.bmp -pix_fmt rgb24 output.gif && gifsicle --delay=5 --loop output.gif > pred.gif && rm output.gif", SAVE_PATH);
	system(buffer);
}
