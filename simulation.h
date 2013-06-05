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

void simulationStep(struct Map *map, int i);

struct Field* moveAnimal(struct Map *map, int x, int y);

void createChild(struct Map *map, int x, int y);

int checkForPrey(struct Map *map, int x, int y);

struct Field* getRandomEmptyNeighboredField(struct Map *map, int x, int y);

struct Field* getNeighboringFieldInDirection(struct Map *map, int x, int y, enum Direction direction);

#endif
