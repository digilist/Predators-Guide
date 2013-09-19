#ifndef RESULTS_H_INCLUDED
#define RESULTS_H_INCLUDED

struct StepResult
{
	unsigned int amount_predators;
	unsigned int amount_prey;
	unsigned int current_step;
	struct StepResult *next;
};

struct SimulationResult
{
	int simulation_steps;
	struct StepResult *first_step_result;
};

void save_result(struct SimulationResult *result);

#endif
