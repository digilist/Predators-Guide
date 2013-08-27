#include "map.h"

#ifndef SIMULATION_H_INCLUDED
#define SIMULATION_H_INCLUDED

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

struct SimulationResult* runSimulation(struct RuntimeConfiguration *config);

struct StepResult* simulationStep(struct Map *map, int i);

#endif
