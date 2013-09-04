#ifndef RESULTS_H_INCLUDED
#define RESULTS_H_INCLUDED

struct StepResult
{
	int amount_predators;
	int amount_prey;
	int current_step;
	struct StepResult *next;
};

struct SimulationResult
{
	int simulationSteps;
	struct StepResult *firstStepResult;
};

void saveResult(struct SimulationResult *result);

#endif
