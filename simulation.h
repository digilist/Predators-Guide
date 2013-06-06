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

struct Field* checkForPrey(struct Map *map, struct Field *field);

struct Field* moveAnimal(struct Map *map, struct Field *field);

void createChild(struct Map *map, struct Field *field);

struct Field* getRandomNeighboredField(struct Map *map, struct Field *field);

struct Field* getRandomEmptyNeighboredField(struct Map *map, struct Field *field);

struct Field* getNeighboringFieldInDirection(struct Map *map, int x, int y, enum Direction direction);

#endif
