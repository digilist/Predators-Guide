#include "map.h"
#include "result.h"

#ifndef SIMULATION_H_INCLUDED
#define SIMULATION_H_INCLUDED

/**
 * Richtungen in die sich ein Tier bewegen kann
 *
 * auf keinen Fall die Reihenfolge ändern!
 */
enum Direction
{
	UP, DOWN, LEFT, RIGHT, UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT, NUMBER_OF_DIRECTIONS
};

void simulation_step(int step);
struct StepResult* calculate_step_result(int step);

#endif
