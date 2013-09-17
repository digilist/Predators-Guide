#include "map.h"
#include "result.h"

#ifndef SIMULATION_H_INCLUDED
#define SIMULATION_H_INCLUDED

void simulation_step(struct Map *map, struct Segment *segment, int step);

struct StepResult* calculate_step_result(struct Map *map, struct Segment *segment, int step);

#endif
