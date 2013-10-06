#ifndef RESULTS_H_INCLUDED
#define RESULTS_H_INCLUDED

#include <sys/time.h>

struct StepResult
{
	unsigned int amount_predators;
	unsigned int amount_prey;
	unsigned int amount_plants;
	unsigned int current_step;
	unsigned long operations;
	struct StepResult *next;
};

struct SimulationResult
{
	int simulation_steps;
	struct StepResult *first_step_result;
	unsigned long operations;
	struct timeval start_time;
	struct timeval finish_time;
	struct timeval run_time;
};

void save_result(struct SimulationResult *result);

#endif
