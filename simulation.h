#include "map.h"
#include "result.h"

#ifndef SIMULATION_H_INCLUDED
#define SIMULATION_H_INCLUDED

struct SimulationResult* runSimulation();

struct StepResult* simulationStep(struct Map *map, int i);

#endif
