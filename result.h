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
	int simulationSteps;
	struct StepResult *firstStepResult;
};

void saveResult(struct SimulationResult *result);

#endif
