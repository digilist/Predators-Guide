#include "map.h"

#ifndef SIMULATION_H_INCLUDED
#define SIMULATION_H_INCLUDED

/**
 * Richtungen in die sich ein Tier bewegen kann
 *
 */
enum Direction {
	UP, DOWN, LEFT, RIGHT
};

void simulationStep(struct Map *map);

void moveAnimal(struct Map *map, int i, int j);

void createChild(struct Map *map, int i, int j);

int checkForPrey(struct Map *map, int i, int j);

struct Field* getNeighboringFieldInDirection(struct Map *map, int x, int y, enum Direction direction);

#endif
